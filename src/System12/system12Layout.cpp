#include "System12/Layout.h"

namespace System12 {
	
using namespace nw4r::lyt;

Layout::Layout(EGG::Heap* pHeap) 
	: _00(nullptr)
	, _04(nullptr)
	, _08(nullptr)
	, _0C(nullptr)
	, _10(nullptr)
	, _1C(nullptr)
{
	mHeap = pHeap;
}


bool System12LayoutImpl::Build(const void* pLytBinary, ResourceAccessor* pResAcsr) {
    const res::BinaryFileHeader* const pHeader =
        static_cast<const res::BinaryFileHeader*>(pLytBinary);

    if (!detail::TestFileHeader(*pHeader, SIGNATURE)) {
        return false;
    }
	
	if (!detail::TestFileVersion(*pHeader)) {
        return false;
    }

    ResBlockSet blockSet = {
        nullptr,     // pTextureList
        nullptr,     // pFontList
        nullptr,     // pMaterialList
        pResAcsr // pResAccessor
    };

    Pane* pParentPane = nullptr;
    Pane* pLastPane = nullptr;

    bool bReadRootGroup = false;
    int groupNestLevel = 0;

    const void* pBlockData =
        static_cast<const u8*>(pLytBinary) + pHeader->headerSize;

    for (int i = 0; i < pHeader->dataBlocks; i++) {
        const res::DataBlockHeader* pBlockHeader =
            static_cast<const res::DataBlockHeader*>(pBlockData);

        switch (detail::GetSignatureInt(pBlockHeader->kind)) {
        case 'lyt1': {
            const res::Layout* pRes =
                static_cast<const res::Layout*>(pBlockData);

            mLayoutSize = pRes->layoutSize;
            break;
        }

        case 'txl1': {
            blockSet.pTextureList =
                static_cast<const res::TextureList*>(pBlockData);
            break;
        }

        case 'fnl1': {
            blockSet.pFontList = static_cast<const res::FontList*>(pBlockData);
            break;
        }

        case 'mat1': {
            blockSet.pMaterialList =
                static_cast<const res::MaterialList*>(pBlockData);
            break;
        }

        case 'pan1':
        case 'pic1':
        case 'txt1':
        case 'wnd1':
        case 'bnd1': {
            Pane* pPane =
                buildPane(detail::GetSignatureInt(pBlockHeader->kind),
                             pBlockData, blockSet);

            if (pPane != nullptr) {
                if (mpRootPane == nullptr) {
                    mpRootPane = pPane;
                }

                if (pParentPane != nullptr) {
                    pParentPane->AppendChild(pPane);
                }

                pLastPane = pPane;
            }

            break;
        }
		
		case 'usd1': {
            pLastPane->SetExtUserDataList(static_cast<const res::ExtUserDataList*>(pBlockData));
            break;
		}

        case 'pas1': {
            pParentPane = pLastPane;
            break;
        }

        case 'pae1': {
            pLastPane = pParentPane;
            pParentPane = pLastPane->GetParent();
            break;
        }

        case 'grp1': {
            if (!bReadRootGroup) {
                bReadRootGroup = true;
                mpGroupContainer = NewObj<GroupContainer>();
                break;
            }

            if (mpGroupContainer != nullptr && groupNestLevel == 1) {
                Group* pGroup = NewObj<Group>(
                    reinterpret_cast<const res::Group*>(pBlockHeader),
                    mpRootPane);

                if (pGroup != nullptr) {
                    mpGroupContainer->AppendGroup(pGroup);
                }
            }

            break;
        }

        case 'grs1': {
            groupNestLevel++;
            break;
        }

        case 'gre1': {
            groupNestLevel--;
            break;
        }
        }

        pBlockData = static_cast<const u8*>(pBlockData) + pBlockHeader->size;
    }

    return true;
}

Pane* System12LayoutImpl::buildPane(s32 kind, const void* pBinary,
                           const ResBlockSet& rBlockSet) {

    switch (kind) {
    case 'pan1': {
        const res::Pane* pRes = static_cast<const res::Pane*>(pBinary);
        return NewObj<Pane>(pRes);
    }

    case 'pic1': {
        const res::Picture* pRes = static_cast<const res::Picture*>(pBinary);
        return NewObj<Picture>(pRes, rBlockSet);
    }

    case 'txt1': {
        const res::TextBox* pRes = static_cast<const res::TextBox*>(pBinary);
        return NewObj<System12TextBox>(pRes, rBlockSet);
    }

    case 'wnd1': {
        const res::Window* pRes = static_cast<const res::Window*>(pBinary);
        return NewObj<Window>(pRes, rBlockSet);
    }

    case 'bnd1': {
        const res::Bounding* pRes = static_cast<const res::Bounding*>(pBinary);
        return NewObj<Bounding>(pRes, rBlockSet);
    }

    default: {
        return nullptr;
    }
    }
}
}