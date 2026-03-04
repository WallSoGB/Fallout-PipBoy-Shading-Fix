#include "nvse/PluginAPI.h"

NVSEInterface* g_nvseInterface{};

void SafeWrite32(UInt32 addr, UInt32 data)
{
	UInt32	oldProtect;

	VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
	*((UInt32*)addr) = data;
	VirtualProtect((void*)addr, 4, oldProtect, &oldProtect);
}

void ReplaceCall(UInt32 jumpSrc, UInt32 jumpTgt)
{
	SafeWrite32(jumpSrc + 1, jumpTgt - jumpSrc - 1 - 4);
}
template <typename T_Ret = UInt32, typename ...Args>
__forceinline T_Ret ThisStdCall(UInt32 _addr, const void* _this, Args ...args)
{
	return ((T_Ret(__thiscall*)(const void*, Args...))_addr)(_this, std::forward<Args>(args)...);
}

static void __fastcall CullerFrustumFixHook(DWORD* apThis, void*, DWORD* apCamera) {
	apThis[3] = (DWORD)apCamera;
	ThisStdCall(0xA694A0, apThis, apCamera + 55);
}

bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "PipBoyShadingFix";
	info->version = 3;

	return true;
}

bool NVSEPlugin_Load(NVSEInterface* nvse)
{
	if (!nvse->isEditor) {
		ReplaceCall(0x87090A, (UInt32)CullerFrustumFixHook);
	}

	return true;
}
