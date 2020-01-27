
// MFCApplication1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include <cstring>
#include<locale>
#include<codecvt>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication1Dlg 对话框



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMFCApplication1Dlg 消息处理程序

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

class chs_codecvt : public std::codecvt_byname<wchar_t, char, std::mbstate_t> {
public:
	chs_codecvt() : codecvt_byname(".936") { }
};

class euc_codecvt : public std::codecvt_byname<wchar_t, char, std::mbstate_t> {
public:
	euc_codecvt() : codecvt_byname("iso-8859-1") { }
};

std::string url_decode(wchar_t* line)
{

	std::string str;
	int lengths = wcslen(line);
	wchar_t *urlData = new wchar_t[lengths + 1];
	char fmt[4];
	wcsncpy_s(urlData,lengths+1, line, lengths);
	int ulen = 0;
	bool bFoundChar = false;

	int j = 0;

	do
	{
		j = 0;
		bFoundChar = false;
		for (int i = 0; i < lengths; i++)
		{
			wchar_t ch = urlData[i];
			if (ch == L'+')
			{
				str += ' ';
			}
			else if (ch == L'%' && i + 2 < lengths &&  iswxdigit(urlData[i + 1]) && iswxdigit(urlData[i + 2]))
			{
				bFoundChar = true;

				fmt[0] = wctob(urlData[++i]);
				fmt[1] = wctob(urlData[++i]);
				fmt[2] = 0;

				str += (char)strtol(fmt, NULL, 16);

			}
			else {
				str += wctob(ch);
			}
		}
		lengths = j;
	} while (bFoundChar);


	delete urlData;


	return str;

}

void CMFCApplication1Dlg::OnDropFiles(HDROP hDropInfo)
{
	unsigned int file_count;
	unsigned int file_start;
	wchar_t filepath[MAX_PATH];
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> ucs2conv;
	file_count= DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	std::string binstr;
	std::wstring filename_out;
	bool noerror = true;
	if (DragQueryFile(hDropInfo, 0, filepath, MAX_PATH) > 0) {
		CString newfile(filepath),filename,path;
		file_start=newfile.ReverseFind('\\');
		filename = newfile.Mid(file_start + 1);
		path = newfile.Mid(0, file_start);
		std::wstring newfilepath;

		std::wstring_convert<chs_codecvt> CHSconverter;
		std::wstring_convert<euc_codecvt> EUCconverter;
		try {
			binstr = EUCconverter.to_bytes(filename);
			filename_out = CHSconverter.from_bytes(binstr);
		}
		catch (const std::range_error& e) {
			MessageBox(L"无法转换文件名", (reinterpret_cast<LPCTSTR>(L"HEllo")), MB_ICONERROR);
			noerror = false;
		}
		
		if ((wcscmp(filename, filename_out.c_str()) == 0) && (wcschr(filename,'%')>0)) {
			std::string urlfilename = url_decode(filename.GetBuffer());
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> ucs2conv;
			try {
				filename_out = ucs2conv.from_bytes(urlfilename);
			}
			catch (const std::range_error& e) {
				MessageBox(L"无法转换文件名", (reinterpret_cast<LPCTSTR>(L"HEllo")), MB_ICONERROR);
				noerror = false;
			}
		}
		newfilepath = path + '\\';
		newfilepath += filename_out;
		
		if ((filename_out.length() > 0 )&& noerror) {
			if (MessageBox(newfilepath.c_str(), (reinterpret_cast<LPCTSTR>(L"转换文件名为")), MB_ICONINFORMATION || MB_OKCANCEL) == IDOK) {
				_wrename(newfile, newfilepath.c_str());
			}
		}
		else {
			MessageBox(L"无法完成转换文件", (reinterpret_cast<LPCTSTR>(L"无法转换")), MB_ICONWARNING);
		}
	}
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnDropFiles(hDropInfo);
}
