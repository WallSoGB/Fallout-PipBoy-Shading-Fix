#include "nvse/PluginAPI.h"

NVSEInterface* g_nvseInterface{};

void PatchMemoryNop(ULONG_PTR Address, SIZE_T Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (SIZE_T i = 0; i < Size; i++)
		*(volatile BYTE*)(Address + i) = 0x90; //0x90 == opcode for NOP

	VirtualProtect((LPVOID)Address, Size, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "PipBoyShadingFix";
	info->version = 1;

	return true;
}

bool NVSEPlugin_Load(NVSEInterface* nvse)
{
	if (!nvse->isEditor) {
		PatchMemoryNop(0x87091F, 5);
	}

	return true;
}
