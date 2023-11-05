#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Entity.h"
#include "Command.h"
#include "CreateCmd.h"
#include "Windows.h"
#include "TextInputDlg.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInsertComb::CInsertComb()
	: m_LeftTop(0, 0), m_RightBottom(0, 0)
{
	m_nStep = 0; // ��ʼ��������Ϊ 0
}

CInsertComb::~CInsertComb()
{
}

int CInsertComb::GetType()
{
	return ctCreateComb;
}

int	CInsertComb::OnLButtonDown(UINT nFlags, const Position& pos)
{
	CTextInputDlg dlg1, dlg2;
	dlg1.DoModal();
	dlg2.DoModal();

	CComb* oldComb = NULL;    // �½�һ��CComb���͵�ָ���������ֵΪNULL
	POSITION position = g_pDoc->m_EntityList.GetHeadPosition();  // position���ڴ���ͼԪ����Ŀ�ʼ��ַ��
	while (position != NULL) {  // ��position��Ϊ�գ���ͼԪ�����д���������ʱ������whileѭ��
		MEntity* entity = (MEntity*)g_pDoc->m_EntityList.GetAt(position);  // ����ͼԪ�ĵ�ַѭ������ͼԪ
		if (entity->GetType() == etComb) {  // ���entity����������etComb�����if
			CComb* comb = (CComb*)entity;  // Ϊ�˱���entity����entity������comb
			if (!comb->GetName().Compare(dlg1.m_text)) // ��comb��name��dlg1�Ի�����������ֽ��жԱ�
			{
				oldComb = (CComb*)entity;   // ��comb��name��dlg1�Ի������������һ��ʱ����entity������oldComb
				break;
			}
		}
		g_pDoc->m_EntityList.GetNext(position); // ǰ����һ��ͼԪ�ĵ�ַ
	}
	if (oldComb == NULL) {
		char msg[256];
		sprintf(msg, "���������: %s", dlg1.m_text);
		MessageBox(NULL, msg, "����", MB_OK);
		return 0;
	}

	std::vector<MEntity*> newEntities;  // �½�һ����������newEntities
	for (MEntity* entity : oldComb->GetEntities()) {
		newEntities.push_back(entity->Copy());
	}
	CComb* newComb = new CComb(dlg2.m_text, oldComb->GetLeftTop(),oldComb->GetRightBottom(),newEntities);
	newComb->Move(oldComb->GetLeftTop(), pos);
	CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ�� 
	newComb->Draw(pDC, dmNormal);
	g_pDoc->m_EntityList.AddTail(newComb); // ��ָ����ӵ�ͼԪ����
	g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
	newComb->m_nOperationNum = g_pView->m_nCurrentOperation;
	g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��

	return 0;
}

int	CInsertComb::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_RECT));

	// ��һ��̬����nPreRefresh��¼����OnMouseMove״̬ʱ��ˢ�´���
	static	int nPreRefresh = g_nRefresh;
	// ��������bRefresh˵����OnMouseMove�������Ӵ��Ƿ�ˢ��
	BOOL	bRefresh = FALSE;
	// nCurRefresh���ڼ�¼��ǰ��ˢ�´���
	int		nCurRefresh = g_nRefresh;
	// ���nCurRefresh��nPreRefresh����ȣ�˵���Ӵ�����ˢ�¹�
	if (nCurRefresh != nPreRefresh) {
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh;
	}

	switch (m_nStep)
	{
	case 0:
		::Prompt("��������ε����Ͻǵ㣺");
		break;
	case 1:
	{
		Position	prePos, curPos;
		prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��

		curPos = pos;

		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��

		// ������ʱ���������һ����Ƥ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, prePos);
		if (!bRefresh) // ���Ӵ�û�б�ˢ��ʱ���ػ�ԭ������Ƥ��ʹ�䱻����
			pTempRect->Draw(pDC, dmDrag);
		delete pTempRect;
		// ������ʱ���󣬸��ݵ�ǰλ�û���һ����Ƥ��
		MRectangle* pTempRect2 = new MRectangle(m_LeftTop, curPos);
		pTempRect2->Draw(pDC, dmDrag);
		delete pTempRect2;

		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��			

		m_RightBottom = curPos; // ����ǰλ������Ϊֱ���յ㣬�Ա���һ������ƶ�ʱ��
		break;
	}
	}
	return 0;
}
// ��������Ҽ�ȡ����ǰ�Ĳ���
int	CInsertComb::OnRButtonDown(UINT nFlags, const Position& pos)
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempRect;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("��������ε����Ͻǵ㣺");
	return 0;
}
// ����Cancel ����ȡ�����β���
int CInsertComb::Cancel()
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempRect;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("����"); // �ȴ���ʾ�����͵��������
	return 0;
}

