#pragma once

#include <Windows.h>
#include <wrl.h>
#include <metahost.h>
#include <cstdint>
#include <vector>
#include <memory>

#import <mscorlib.tlb> raw_interfaces_only				\
    high_property_prefixes("_get","_put","_putref")		\
    rename("ReportEvent", "InteropServices_ReportEvent")

namespace clr {
    constexpr wchar_t clr_default_version[] = /*L"v2.0.50727"; */L"v4.0.30319";

    class ClrAssembly {
    private:
        mscorlib::_AssemblyPtr p_;

    public:
        ClrAssembly(mscorlib::_AssemblyPtr p);
        bool construct(const std::wstring& classname, variant_t& var);
    };

    class ClrDomain {
    private:
        Microsoft::WRL::ComPtr<ICLRMetaHost>	pMeta_;
        Microsoft::WRL::ComPtr<ICLRRuntimeInfo> pRuntime_;
        Microsoft::WRL::ComPtr<ICorRuntimeHost> pHost_;
        std::vector<std::shared_ptr<SAFEARRAY>>	arr_;
    public:
        ClrDomain();
        ClrDomain(const std::wstring& clrTarget);
        ~ClrDomain();
        std::unique_ptr<ClrAssembly> load(std::vector<uint8_t>& mod);
    };
}

