// dllmain.h : Declaration of module class.

class CcomNorsynObjectsModule : public ATL::CAtlDllModuleT< CcomNorsynObjectsModule >
{
public :
	DECLARE_LIBID(LIBID_comNorsynObjectsLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMNORSYNOBJECTS, "{7c99a33f-a411-48d6-9707-81badecc45f7}")
};

extern class CcomNorsynObjectsModule _AtlModule;
