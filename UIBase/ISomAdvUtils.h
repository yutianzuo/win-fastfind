#pragma once

#include "plugin.h"
#include "kernl.h"

// �Ƿ���ʾ����ʱ��С���ֵ�����·��		����ΪDWORD  0:��ʾ����ʾ ��0��ʾ��ʾ
#define	SPEED_ASSIST_SHOW_KEY	L"SpeedAssist\\IsShow"

#define ALL_ITEM_KEY				-1	// �������е�Item 
#define RUN_SOFT_ALL_BIT			0xFFFFFFFF		// ������Ϣ��λ����
#define RUN_SOFT_DESCRIPTION_BIT	0x00000001		// ������Ϣ��λ����
#define RUN_SOFT_SECURITY_BIT		0x00000002		// ��ȫ��Ϣ��λ����

class IStUpInfoEx ;

// ֪ͨ��Ϣ����
enum ENotifyMsgType
{
	eNotifyUnknown = 0,
	eNotifyAdd,			// �������
	eNotifyDel,			// ɾ������
	eNotifyChange,		// ������ݸı���
	eNotifyBaseLoad,	// ������Ϣ�������
	eNotifyDelayLoad,	// �ӳ���Ϣ�������
	eNotifyBeginDloadCfg,	// ��ʼ����������Ϣ
	eNotifyEndDloadCfg,		// ��������������Ϣ
};

// ֪ͨ��Ϣ���� 
struct NOTIFY_MSG_PARAM
{	
	DWORD	dwKey;			// ���key	 ALL_ITEM_KEY��ʾ���еĶ��ı��� ��֧�����ɸ���
	LONG	lDelayLoadBit;	// �ӳټ������Ե�λ���� ÿ������һλ
};

// ˵��: ���п��Ż������ ������(StartUp) ����� ����(Service) ����ƻ�(Task) run��(Run)
// 

// ������Ŀ ����������
enum EAutoRunStartType
{
	eStartUnknown = -1,
	eStartDisabled = 0,		// ��ֹ����
	eStartDelay,			// �ӳ�����
	eStartAuto,				// �Զ�����
	eStartDelete,			// ɾ�������� ֻ�������������ƻ�֧��
	eStartIgnore,			// ���ʱ����ĳ��
};

// run�������
enum ERunItemType
{
	eRunUnknown = -1,
	eRunAllUserStartUp = 0,
	eRunCurUserStartUp,
	eRunLMRun,
	eRunCURun,
	eRunLMRunOnce,
	eRunCURunOnce,
	eRunLMRun64,
	eRunLMRunOnce64,
};

// ����������
enum EStartUpType	
{
	eAtRunUnKnown = 0,
	eAtRunMin,
	eAtRunService,		// ����
	eAtRunTask,			// ����ƻ�
	eAtRunRun,			// ��ʼ��
	eAtRunDefault,		// Ĭ����
	eAtRunHistroy,		// ������ʷ
	eAtRunRelation,		// ���������
	eAtRunSystem,		// ϵͳ��
	eAtRunDeepScan,     // ľ���Ʋ�ɱ������
	eAtRunMax,			// �������
	eAtRunOnlyBanned,   // ֻ��ȡ�����Լ������ֹ����Ϣ
};

// ��������
enum EAdviceType
{
	eAdcAllowDisable = 0,	//�����ֹ		
	eAdcAdviseRun,		//��������
	eAdcMustRun,		//��������
	eAdcAdviseDelay,	//�����ӳټ���
	eAdcAdviseKeep,		//���鱣�����ڵ�����
	eAdcUnknown = 99,
};

// �������
enum ESoftType
{
	etAVSoft     = 5, // ɱ�����
	etChatSoft   = 17, // �������
	etCommonSoft = 0, // ��ͨ���
};

// ����ϵͳ��Ϣ
enum ESystemVersion
{
	evUnknown       = 0x00000000, 
	evWindowsALL    = 0x00000001,
	evWindows64     = 0x00000002,
	evWindows2000   = 0x00000004,
	evWindowsXP     = 0x00000008,
	evWindows2003   = 0x00000010,
	evWindowsVista  = 0x00000020,
	evWindows7      = 0x00000040,
};

// ���֧������
enum EDialogSupportType
{
	stNotWantDialog    = 0, // ����Ҫ�κζԻ���
	stSetPicDialog     = 1, // ��ͼƬ�����öԻ���
	setUninstallDialog = 2, // ����ͼƬ��ж�ضԻ���
};

interface SOM_NO_VTABLE ISomRunInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_INFO };

	STDMETHOD(GetFileName)(ISomString** psFileName) PURE;
	STDMETHOD(GetDisplayName)(ISomString** psDispName) PURE;
	STDMETHOD(GetFilePath)(ISomString** psFilePath) PURE;
	STDMETHOD(GetDescription)(ISomString** psDescription) PURE;
	STDMETHOD(GetCmdLine)(ISomString** psCmdLine) PURE;
	STDMETHOD(GetParameter)(ISomString** psParam) PURE;
	STDMETHOD_(EAutoRunStartType, GetStartType)() PURE;
	STDMETHOD_(ERunItemType, GetRunType)() PURE;
};

class SOM_NO_VTABLE ISomRun: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_MGR };

	// ö��ϵͳ�������� ���ط�������Ϣ�б�
	STDMETHOD(EnumRuns)(ISomList* plstSvr) PURE;

	// �������������� ��: ���Զ����� ���ǽ�ֹ
	STDMETHOD(SetRunStartType)(LPCWSTR psName, ERunItemType eRunTypem, EAutoRunStartType eNewType, EAutoRunStartType eOldType) PURE;
};

class SOM_NO_VTABLE ISomServiceInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SERIVER_INFO };

	// �õ� ��������
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// �õ� ������ʾ����
	STDMETHOD(GetDispName)(ISomString** psDispName) PURE;

	// �õ� ������Ϣ
	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;

	// �õ� �����в���
	STDMETHOD(GetCmdLine)(ISomString** psCmdLine) PURE;

	// �õ� ·��
	STDMETHOD(GetFilePath)(ISomString** psCmdLine) PURE;

	// �õ� ������������
	STDMETHOD_(EAutoRunStartType, GetStartType)() PURE;

	// �õ� �����Ƿ�����ӳټ���
	STDMETHOD_(BOOL, CanDelayRun)() PURE;
};

class SOM_NO_VTABLE ISomService: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_SERIVER_MGR };

	// ö��ϵͳ�����з��� ���ط�������Ϣ�б�
	STDMETHOD(EnumServices)(ISomList* plstSvr) PURE;

	// ���÷����������� ��: ���Զ����� ���ǽ�ֹ
	// eType:	�������������
	// bForbid:	�÷����Ƿ񱻽�ֹ
	STDMETHOD(SetServiceStartType)(LPCWSTR pszName, EAutoRunStartType eType, BOOL bForbid) PURE;

	// �ָ���ϵͳĬ������
	STDMETHOD(RestoreSystemDef)() PURE;
};

class SOM_NO_VTABLE ISomTaskInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_TASK_INFO };

	// �õ� ����
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// �õ� ��ʾ����
	STDMETHOD(GetDispName)(ISomString** psDispName) PURE;

	// �õ� ������Ϣ
	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;

	// �õ� ��������
	STDMETHOD(GetAppName)(ISomString** psDesp) PURE;

	// �õ� ��������
	STDMETHOD(GetFilePath)(ISomString** psDesp) PURE;

	// �õ� ��������
	STDMETHOD(GetParameters)(ISomString** psDesp) PURE;

	// �õ� �����в���
	STDMETHOD(GetCmdLine)(ISomString** psCmdLine) PURE;

	// �õ� ������������
	STDMETHOD_(EAutoRunStartType, GetStartType)() PURE;
};

class SOM_NO_VTABLE ISomTask: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_TASK_MGR };

	// ö��ϵͳ����������ƻ� ��������ƻ���Ϣ�б�
	STDMETHOD(EnumTask)(ISomList* plstTask) PURE;

	// ��������ƻ��������� Ŀǰ֧��: ���Զ����� ���ǽ�ֹ
	/*
	*	psTaskName: ����ƻ�����
	*	eNewType:	׼�����õ��������� 
	*	eOldType:   �������õ���������
	*/
	STDMETHOD(SetTaskStartType)(LPCWSTR pszName, EAutoRunStartType eNewType, EAutoRunStartType eOldType) PURE;
};

class SOM_NO_VTABLE IStUpInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_AUTO_RUN_ITEM };

	// �õ� ����
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// �õ� ��ʾ��
	STDMETHOD(GetDispName)(ISomString** psName) PURE;

	// �õ� ��������
	STDMETHOD(GetAppName)(ISomString** psAppName) PURE;

	// �õ� �ļ�·��
	STDMETHOD(GetFilePath)(ISomString** psFilePath) PURE;

	// �õ� ����
	STDMETHOD(GetWarning)(ISomString** psWarning) PURE;

	// �õ� ������
	STDMETHOD(GetCmdLine)(ISomString** psCmdLine) PURE;

	// ��� ����������� ��ʾ�Ǽƻ����񣬷�����������
	STDMETHOD_(EStartUpType, GetItemType)() PURE;

	// ��� Run������� ��ʾ����HKLM ���� HKCU �����ڿ�ʼ��˵���
	STDMETHOD_(ERunItemType, GetRunItemType)() PURE;

	//  ��������: ������ ��ֹ�����ӳ� 
	STDMETHOD_(EAutoRunStartType, GetStartType)() PURE;

	// �õ� ����Ŀ�����õ�ʱ��
	STDMETHOD(GetDisabledTime)(time_t* ptmDisabledTime) PURE;

	/// �����Ҫ��ֹ����
	STDMETHOD(GetStartUpParent)(IStUpInfo** pInfo) PURE;

	/// ��ø�����չItem
	STDMETHOD(GetStartUpParentEx)(IStUpInfoEx** pInfo) PURE;

	/// �����Ҫ��ֹ������
	STDMETHOD(GetStartUpChild)(ISomList ** spList) PURE;

	/// �ж��Ƿ�Ϊ����
	STDMETHOD_(BOOL, IsItemRemanet)() PURE;
};

class SOM_NO_VTABLE IStUpInfoEx: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_AUTO_RUN_ITEM_EX };

	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;

	// ��� �������� ����: �ǽ����������ǽ����ֹ
	STDMETHOD_(EAdviceType, GetAdviceType)() PURE;

	// ��� ������� ����: �ǽ����������ǽ����ֹ
	STDMETHOD_(ESoftType, GetSoftType)() PURE;

	// ��öԻ��������
	STDMETHOD_(EDialogSupportType, GetDlgSupportType());

	// ��ø����Rankֵ
	STDMETHOD_(LONG, GetItemRank());

	// ��ȡA-B��صĶԻ����ı���Ϣ
	STDMETHOD(GetRelationText)(ISomString** psInfo) PURE;

	// ��ȡA-B�������Keyֵ����
	STDMETHOD(GetRelationKeys)(ISomStringList** valueVec) PURE;

	// ��ȡ�������������URL
	STDMETHOD(GetSettingPicUrl)(ISomString** psInfo) PURE;

	// ��ȡ����ĳ�����������ʾ��Ϣ
	STDMETHOD(GetForbidenTipInfo)(ISomString** psInfo) PURE;

	// ���ݲ������ó���ȥ����
	STDMETHOD(InvokeItemProgram)() PURE;

	// ж�ظ���
	STDMETHOD(UninstallItem)() PURE;

	// ��ȡ��ϸ��Ϣ�ĶԻ���
	STDMETHOD(GetItemPicUrl)(ISomString** psInfo) PURE;

	// ��ȡ������
	STDMETHOD(GetGroupName)(ISomString** psInfo) PURE;

	// ��ȡ����Ϣ
	STDMETHOD(GetGroupInfo)(ISomString** psInfo) PURE;

	// ��ȡ�ڴ���Ϣ
	STDMETHOD(GetMemoryInfo)(ISomString** psInfo) PURE;

	// �Ƿ���ϵͳ��������
	STDMETHOD_(BOOL, IsSystemItem)() PURE;

	// �Ƿ���Ҫ����ע���
	STDMETHOD_(BOOL, IsLockReg)() PURE;

	// ��ȡ��ID
	STDMETHOD_(LONG, GetGroupID)() PURE;

	// ��ȡ����ID
	STDMETHOD_(LONG, GetRuleID)() PURE;

	// �ܷ�һ���Ż�
	STDMETHOD_(BOOL, CanAutoOptimize)() PURE;
	// �жϸ����Ƿ�Ϊһ���Ż�����
	STDMETHOD_(BOOL, IsAutoOptimize)() PURE;

	/* ������ֻ��Է�����
	* disbleʱ�Ƿ�������Ϊ��ֹ
	* �������ֹʱ�ǽ�������Ϊ�ֶ�����
	* ����Щ�����ֹʱ �ǽ�������Ϊ��ֹ����  
	*/
	STDMETHOD_(BOOL, IsForbidService)() PURE;

	// �õ� �ܷ��ӳټ���
	STDMETHOD_(BOOL, CanDelayRun)() PURE;

	// �õ�������Ϣ
	STDMETHOD(GetAlertInfo)(ISomString** psAlterInfo) PURE;

	// ���ʱ���Դ���
	STDMETHOD(Ignore)() PURE;
	// �����Ƿ��Ѿ����û�����
	STDMETHOD_(BOOL, IsIgnored)() PURE;

};

#define START_OPTMZ_BASE_INFO_COMPLETE -2		// ������Ϣ������
class SOM_NO_VTABLE IStartOptimize: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_START_OPTIMIZE };

	// ע��һ����ѯ������������Ĵ��� 
	// hWnd ΪNULL  �����յ�֪ͨ��Ϣ
	// dwMsgID: ����ע�ᴰ�ڵ���ϢID 
	STDMETHOD(AdviseNotifyWnd)(HWND hWnd, UINT dwMsgID) PURE;
	// ע��һ��������������� Ϊע��ʱ�Ĵ��ھ��
	STDMETHOD(UnadviseNotifyWnd)(HWND hWnd) PURE;

	// ���ע��ɹ� �����̵������������������õ�һЩ��������õ���Ϣ
	// �������������Ŀ ����Ŀ����������������������Ŀ��
	STDMETHOD_(DWORD, GetCount)();
	// ��þ���һ�� 
	STDMETHOD(GetItem)(DWORD dwIndex, IStUpInfo** ppItem) PURE;

	// ͨ��Key��þ���һ�� 
	STDMETHOD(GetItemByKey)(LPCTSTR lpszKey, IStUpInfo** ppItem) PURE;

	// ͨ��Key��þ���һ����ӳ���Ϣ
	STDMETHOD(GetItemExByKey)(LPCTSTR lpszKey, IStUpInfoEx** ppItem) PURE;

	// �ú������յ��ӳټ��سɹ���Ϣ��ſ��Ե���
	STDMETHOD(GetItemEx)(DWORD dwIndex, IStUpInfoEx** ppItemEx) PURE;

	// ���÷��������������
	STDMETHOD(SetItemStartType)(DWORD dwIndex, EAutoRunStartType eStartType) PURE;

	// ˢ�� �����¼������е�������
	STDMETHOD(Refresh)() PURE;

	// �Ե�ǰ������Item��״̬ ����һ������
	STDMETHOD(SnapShot)() PURE;
	// �ָ����е�item���ϴα���Ŀ��յ�״̬
	STDMETHOD(Restore)() PURE;

	// �����е��������һ������Ŀ��� Ҳ����û�������ǵ���������κ��Ż�ǰ������
	STDMETHOD(OriginalSnapShot)() PURE;
	// �ָ���û�н����κ��Ż�ǰ��״̬
	STDMETHOD(OriginalRestore)() PURE;

	// �ָ���ϵͳĬ������
	STDMETHOD(RestoreSystemDef)() PURE;
};


class SOM_NO_VTABLE IStartOptimize2: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_START_OPTIMIZE2 };

	// ע��һ����ѯ������������Ĵ��� 
	// hWnd ΪNULL  �����յ�֪ͨ��Ϣ
	// dwMsgID: ����ע�ᴰ�ڵ���ϢID 
	STDMETHOD(AdviseNotifyWnd)(HWND hWnd, UINT dwMsgID) PURE;

	// ע��һ��������������� Ϊע��ʱ�Ĵ��ھ��
	STDMETHOD(UnadviseNotifyWnd)(HWND hWnd) PURE;

	// ִ�������߳��л�ȡ��������Ϣ��ͨ����ȡ���ؿ⣬����ƥ��
	STDMETHOD(GetCurrentComputerForbbidenCount)(UINT* puTotalStartUp, UINT* puForbiddenCount) PURE;

	// ͬ��������Ϣ�����߳�
	STDMETHOD(SyncLoadInfo)() PURE;

	// ���ע��ɹ� �����̵������������������õ�һЩ��������õ���Ϣ
	// �������������Ŀ ����Ŀ����������������������Ŀ��
	STDMETHOD_(DWORD, GetCount)();
	// ��þ���һ�� 
	STDMETHOD(GetItem)(DWORD dwIndex, IStUpInfo** ppItem) PURE;

	// ͨ��Key��þ���һ�� 
	STDMETHOD(GetItemByKey)(LPCTSTR lpszKey, IStUpInfo** ppItem) PURE;

	// ͨ��Key��þ���һ����ӳ���Ϣ
	STDMETHOD(GetItemExByKey)(LPCTSTR lpszKey, IStUpInfoEx** ppItem) PURE;

	// �ú������յ��ӳټ��سɹ���Ϣ��ſ��Ե���
	STDMETHOD(GetItemEx)(DWORD dwIndex, IStUpInfoEx** ppItemEx) PURE;

	// ���÷��������������
	STDMETHOD(SetItemStartType)(DWORD dwIndex, EAutoRunStartType eStartType) PURE;

	// ˢ�� �����¼������е�������
	STDMETHOD(Refresh)() PURE;

	// �Ե�ǰ������Item��״̬ ����һ������
	STDMETHOD(SnapShot)() PURE;
	// �ָ����е�item���ϴα���Ŀ��յ�״̬
	STDMETHOD(Restore)() PURE;

	// �����е��������һ������Ŀ��� Ҳ����û�������ǵ���������κ��Ż�ǰ������
	STDMETHOD(OriginalSnapShot)() PURE;
	// �ָ���û�н����κ��Ż�ǰ��״̬
	STDMETHOD(OriginalRestore)() PURE;

	// �ָ���ϵͳĬ������
	STDMETHOD(RestoreSystemDef)() PURE;

	// �ָ���ϵͳĬ������
	STDMETHOD(SetFirstEnumItemType)(EStartUpType eType) PURE;

	// ֹͣ���ڽ��е�ɨ��������ѯ
	STDMETHOD(Stop)() PURE;
};

// �������̵õ�������
class SOM_NO_VTABLE IRunSoftInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_SOFT_INFO };

	// �õ� ���ý�������
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// �õ� ���ÿ�ִ���ļ��ľ���·��
	STDMETHOD(GetFilePath)(ISomString** psFilePath) PURE;

	// �õ� ���ý���ID
	STDMETHOD_(DWORD, GetPID)() PURE;

	// ��� ���ÿ�ִ���ļ��İ汾
	STDMETHOD(GetFileVersion)(ISomString** psVers) PURE;

	// ��� ����������˾
	STDMETHOD(GetCompanyName)(ISomString** psCmpy) PURE;

	// �õ� ���ý���ռ�õ��ڴ��С ��λ�ֽ�
	STDMETHOD_(DWORD, GetMemorySize)() PURE;

	// �Ƿ���ϵͳ�Ľ���
	STDMETHOD_(BOOL, IsSystemProc)() PURE;
};

// �ӳٷ��ص�����
class SOM_NO_VTABLE IRunSoftInfoEx: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_SOFT_INFO_EX };

	// �õ� ����������Ϣ
	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;


	// �õ� ���ð�ȫ����
	STDMETHOD(GetSecurityLevel)(ISomString** psLevel) PURE;
};

class SOM_NO_VTABLE IModuleInfo: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_PROC_MODULE_INFO };

	// �õ� ����ģ������
	STDMETHOD(GetName)(ISomString** psName) PURE;

	// �õ� ����ģ���ļ��ľ���·��
	STDMETHOD(GetFilePath)(ISomString** psFilePath) PURE;

	// �õ� ����������Ϣ
	STDMETHOD(GetDescription)(ISomString** psDesp) PURE;

	// ��� ���ÿ�ִ���ļ��İ汾
	STDMETHOD(GetFileVersion)(ISomString** psVers) PURE;

	// ��� ����������˾����
	STDMETHOD(GetCompanyName)(ISomString** psVers) PURE;
};

// ���������������ӿ�
class SOM_NO_VTABLE IRunSoftMgr: public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_RUN_SOFT_MGR };

	// ע��һ����ѯ������������Ĵ��� 
	// hWnd ΪNULL  �����յ�֪ͨ��Ϣ
	// dwMsgID: ����ע�ᴰ�ڵ���ϢID 
	STDMETHOD(AdviseNotifyWnd)(HWND hWnd, UINT dwMsgID) PURE;
	// ע��һ��������������� Ϊע��ʱ�Ĵ��ھ��
	STDMETHOD(UnadviseNotifyWnd)(HWND hWnd) PURE;

	// ���ע��ɹ� �����̵������������������õ�һЩ��������õ���Ϣ
	// �������������Ŀ ����Ŀ����������������������Ŀ��
	STDMETHOD_(DWORD, GetCount)();
	// ��þ���һ�� 
	STDMETHOD(GetItem)(DWORD dwIndex, IRunSoftInfo** ppItem) PURE;

	// �ú������յ��ӳټ��سɹ���Ϣ��ſ��Ե���
	STDMETHOD(GetItemEx)(DWORD dwIndex, IRunSoftInfoEx** ppItemEx) PURE;

	// ����һ���������еĳ��� 
	// dwIndex:	�����������
	// pbRet:	�Ƿ�ɹ�����
	STDMETHOD(TerminateTask)(DWORD dwIndex, BOOL* pbRet) PURE;

	// ˢ�� �����¼����������е������Ŀ
	STDMETHOD(Refresh)() PURE;

	// ��ѯ���ص�һ�������е�����DLL��Ϣ
	// dwIndex:	�����������
	// pSListΪ ISomModuleInfo�б�
	STDMETHOD(QueryProcModules)(DWORD dwIndex, ISomList* pSList) PURE;
};

// ��������
enum assoc_catalog
{
	assoc_catalog_browser	= 0,
	assoc_catalog_mail		= 1,
	assoc_catalog_video		= 2,
	assoc_catalog_music		= 3,
	assoc_catalog_picture	= 4,
	assoc_catalog_ime		= 5
};

class SOM_NO_VTABLE IAssociation : public ISomPlugin
{
public:
	enum { PLUGINID = SOMPLUGIN_ASSOCIATION };

	STDMETHOD_(int, BeginRefreshSoftwareList)(LPCWSTR lpXmlPath) PURE;
	STDMETHOD_(int, GetSoftwareIdAll)(int index) PURE;
	STDMETHOD_(bool, SetSoftwareInstalled)(int index, int version1, int version2, LPCWSTR lpIcon = NULL) PURE;
	STDMETHOD_(bool, EndRefreshSoftwareList)() PURE;

	STDMETHOD_(int, GetSoftwareCount)(int catalog) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareName)(int catalog, int index) PURE;
	STDMETHOD_(int, GetSoftwareId)(int catalog, int index) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareIcon)(int catalog, int index) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareUninstallKey)(int catalog, int index) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareUninstallName)(int catalog, int index) PURE;

	STDMETHOD_(bool, ExecuteSoftware)(int catalog, int index) PURE;	
	STDMETHOD_(int, GetSoftwareExtCount)(int catalog, int index) PURE;
	STDMETHOD_(LPCWSTR, GetSoftwareExt)(int catalog, int index, int extindex) PURE;
	STDMETHOD_(bool, GetSoftwareExtAssoc)(int catalog, int index, int extindex) PURE;

	STDMETHOD_(int, GetDefaultSoftware)(int catalog) PURE;
	STDMETHOD_(bool, SetDefaultSoftware)(int catalog, int index) PURE;

	STDMETHOD (GetSoftwareDebugInfo)(int catalog, int index, ISomString** ppOutInfo) PURE;
};