/* Copyright (C) Eric Dybsand, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2001"
 */
// TestFuSMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GameGems.h"
#include "TestFuSMDlg.h"
#include "FuSMclass.h"
#include "FuSMstate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// strings for display of IDs during testing
#define FUSM_ID_STRING_LENGTH 15
char* pzFuSMStringIDs[NUMBER_OF_IDS_USED] = {
"               ",
"Berserk        ",
"Raged          ",
"Mad            ",
"Annoyed        ",
"Sad			",	
"Uncaring       ",
"Player Seen    ",
"Player Attacks ",
"Player Gone    ",
"Monster Hurt   ",
"Monster Healed "
};

#if LOG_FILE
extern FILE *fpDebug;			// log file
#endif

/////////////////////////////////////////////////////////////////////////////
// TestFuSMDlg dialog


TestFuSMDlg::TestFuSMDlg(CWnd* pParent, FuSMclass *pFuSMclass)
	: CDialog(TestFuSMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TestFuSMDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pFuSMclass = pFuSMclass;
}


void TestFuSMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TestFuSMDlg)
	DDX_Control(pDX, IDC_LIST1, m_lbStates);
	DDX_Control(pDX, IDC_INPUTSCOMBO, m_cbInputs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestFuSMDlg, CDialog)
	//{{AFX_MSG_MAP(TestFuSMDlg)
	ON_CBN_SELCHANGE(IDC_INPUTSCOMBO, OnSelchangeInputscombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestFuSMDlg message handlers

BOOL TestFuSMDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// initialize input values list box
	m_cbInputs.ResetContent();

	// go through the list of possible inputs and add them to the dropdown combo box
	CString sLine;
	for( int i=INPUT_ID_PLAYER_SEEN; i<NUMBER_OF_IDS_USED; ++i )
	{
		// get the string associated with this input value
		sLine = pzFuSMStringIDs[i];
		int nSel = m_cbInputs.AddString((LPCTSTR)sLine.GetString());

		// store the id of the input string for access later
		m_cbInputs.SetItemData(nSel, (DWORD)i);
		sLine.Empty();
	}

	// initialize state list box
	m_lbStates.ResetContent();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TestFuSMDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// OnSelchangeInputscombo() 
// find the input, apply the input to the states determining which
// are active and to what degree, and then add the active states
// to the state list box
/////////////////////////////////////////////////////////////////////////////

void TestFuSMDlg::OnSelchangeInputscombo() 
{
	// get input value id from combo box
	int iSel = m_cbInputs.GetCurSel();
	if( iSel < 0 )
		return;
	int iID = (int)m_cbInputs.GetItemData(iSel);

	// NOTE: in order to remain totally compatible with the FSM usage
	// (found in TestFSMDlg) of the enum, and to keep the example as
	// simple as possible, we are going to convert the input action 
	// id (iID above) into a rating input to use later.  You may wish
	// to deploy a different approach to determine the value of an
	// input action.
//
// create a discrete "dislike rating" input for action inputs
// positive = dislike more, negative = dislike less
int iDislikeInput = 0;
srand(time(0));
int randomNoise = (rand() % 10 + 1) - 10; // add -5 to 5 random "noise"
switch (iID)
{
case INPUT_ID_PLAYER_SEEN:
	iDislikeInput = 10 + randomNoise;
	break;
case INPUT_ID_PLAYER_ATTACKS:
	iDislikeInput = 30 + randomNoise;
	break;
case INPUT_ID_PLAYER_GONE:
	iDislikeInput = -10 + randomNoise;
	break;
case INPUT_ID_MONSTER_HURT:
	iDislikeInput = +50 + randomNoise;
	break;
case INPUT_ID_MONSTER_HEALED:
	iDislikeInput = -20 + randomNoise;
default:
	break;
}


// make sure the active state list box is cleared
m_lbStates.ResetContent();

// apply the fuzzy input value (the "dislike rating" from above)
m_pFuSMclass->StateTransition(iDislikeInput);

FuSMstate *pFuzzyState = NULL;
// now fill the active state list box with active states
CString sLine;
CString sHow;
WCHAR szLine[FUSM_ID_STRING_LENGTH * 4];

pFuzzyState = NULL;
while ((pFuzzyState = m_pFuSMclass->GetNextFuzzyStateMember()) != NULL)
{
	// crudely translate the degree of membership into a string description
	if (pFuzzyState->GetDegreeOfMembership() < 25)
	{
		sHow = "is Sort of ";
	}
	else if (pFuzzyState->GetDegreeOfMembership() < 50)
	{
		sHow = "is Partially ";
	}
	else if (pFuzzyState->GetDegreeOfMembership() < 75)
	{
		sHow = "is Mostly ";
	}
	else if (pFuzzyState->GetDegreeOfMembership() < 99)
	{
		sHow = "is Very ";
	}
	else
	{
		sHow = "is Totally ";
	}


	// construct a line showing degree of membership and name of state
	CString status = pzFuSMStringIDs[pFuzzyState->GetID()];
	wsprintf((LPWSTR)szLine, TEXT("%d -> %s %s"), pFuzzyState->GetDegreeOfMembership(),
		(const char *)sHow.GetString(), (const char *)status.GetString());

	// and add that line of text to the list box
	iSel = m_lbStates.AddString((LPCTSTR)szLine);
	sHow.Empty();
	status.Empty();
}


//Defuzzify
std::map<int, int> memberships = std::map<int, int>();

memberships.insert(std::pair<int, int>(STATE_ID_UNCARING, m_pFuSMclass->GetState(STATE_ID_UNCARING)->GetDegreeOfMembership()));
memberships.insert(std::pair<int, int>(STATE_ID_SAD, m_pFuSMclass->GetState(STATE_ID_SAD)->GetDegreeOfMembership()));
memberships.insert(std::pair<int, int>(STATE_ID_ANNOYED, m_pFuSMclass->GetState(STATE_ID_ANNOYED)->GetDegreeOfMembership()));
memberships.insert(std::pair<int, int>(STATE_ID_MAD, m_pFuSMclass->GetState(STATE_ID_MAD)->GetDegreeOfMembership()));
memberships.insert(std::pair<int, int>(STATE_ID_RAGE, m_pFuSMclass->GetState(STATE_ID_RAGE)->GetDegreeOfMembership()));
memberships.insert(std::pair<int, int>(STATE_ID_BERSERK, m_pFuSMclass->GetState(STATE_ID_BERSERK)->GetDegreeOfMembership()));



CString actionText;
//Bunch of cases now for the memberships
if (memberships.at(STATE_ID_SAD) >= 50 && memberships.at(STATE_ID_SAD) < 100) {
	actionText = "Monster Weeps in place and backs away";
}
else if (memberships.at(STATE_ID_ANNOYED) >= 50 && memberships.at(STATE_ID_ANNOYED) < 100) {
	actionText = "Monster starts squaring up to Player";
}
else if (memberships.at(STATE_ID_ANNOYED) <= 50 && memberships.at(STATE_ID_SAD) >= 1 && memberships.at(STATE_ID_ANNOYED) > 0) {
	actionText = "Monsters starts looking at player with a tear in it's eye";
}
else if (memberships.at(STATE_ID_ANNOYED) <= 20 && memberships.at(STATE_ID_SAD) >= 1 && memberships.at(STATE_ID_ANNOYED) > 0) {
	actionText = "Monster cowers back a little at the player";
}
	else if (memberships.at(STATE_ID_MAD) >= 10 && memberships.at(STATE_ID_RAGE) >= 1 && memberships.at(STATE_ID_RAGE) < 10) {
		actionText = "Monster does a light attack";
	}
	else if (memberships.at(STATE_ID_MAD) >= 30) {
		actionText = "Monster Prepares to attack the Player";
	}
	else if (memberships.at(STATE_ID_MAD) >= 10) {
		actionText = "Monster looks for weaknesses in the Player";
	}
	else if (memberships.at(STATE_ID_RAGE) >35) {
		actionText = "Monster does a super heavy attack";
	}
	else if (memberships.at(STATE_ID_RAGE) > 10) {
		actionText = "Monster does a heavy attack";
	}
	else if (memberships.at(STATE_ID_BERSERK) > 1) {
		actionText = "Monster goes berserk!";
	}

	wsprintf((LPWSTR)szLine, TEXT("%s"),actionText);




	// and add that line of text to the list box
	iSel = m_lbStates.AddString((LPCTSTR)szLine);

}

// end of TestFuSMDlg.cpp
