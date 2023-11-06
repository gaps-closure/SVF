#include "SVF-LLVM/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/CommandLine.h"
#include "Util/Options.h"
#include "WPA/Steensgaard.h"
#include "SABER/SaberSVFGBuilder.h"

using namespace std;
using namespace SVF;

std::string nodeKindToString(SVF::s64_t kind) {
    switch(kind)
    {
    case VFGNode::Addr:
        return "Addr";
    case VFGNode::Copy:
        return "Copy";
    case VFGNode::Gep:
        return "Gep";
    case VFGNode::Store:
        return "Store";
    case VFGNode::Load:
        return "Load";
    case VFGNode::Cmp:
        return "Cmp";
    case VFGNode::BinaryOp:
        return "BinaryOp";
    case VFGNode::UnaryOp:
        return "UnaryOp";
    case VFGNode::Branch:
        return "Branch";
    case VFGNode::TPhi:
        return "TPhi";
    case VFGNode::TIntraPhi:
        return "TIntraPhi";
    case VFGNode::TInterPhi:
        return "TInterPhi";
    case VFGNode::MPhi:
        return "MPhi";
    case VFGNode::MIntraPhi:
        return "MIntraPhi";
    case VFGNode::MInterPhi:
        return "MInterPhi";
    case VFGNode::FRet:
        return "FRet";
    case VFGNode::ARet:
        return "ARet";
    case VFGNode::AParm:
        return "AParm";
    case VFGNode::FParm:
        return "FParm";
    case VFGNode::APIN:
        return "APIN";
    case VFGNode::APOUT:
        return "APOUT";
    case VFGNode::FPIN:
        return "FPIN";
    case VFGNode::FPOUT:
        return "FPOUT";
    case VFGNode::NPtr:
        return "NPtr";
    case VFGNode::DummyVProp:
        return "DummyVProp";
    }
}  

int main(int argc, char ** argv)
{
    std::vector<std::string> llModules;
    llModules.push_back(argv[1]);

    SVFModule* svfModule = LLVMModuleSet::buildSVFModule(llModules);

    auto mset = LLVMModuleSet::getLLVMModuleSet();


    /// Build Program Assignment Graph (SVFIR)
    SVFIRBuilder builder(svfModule);
    SVFIR* pag = builder.build();

    /// Create pointer analysis
    BVDataPTAImpl* wpa = AndersenWaveDiff::createAndersenWaveDiff(pag);

    SaberSVFGBuilder svfgBuilder;
    SVFG* svfg = svfgBuilder.buildFullSVFG(wpa);

    std::ofstream nodeDump;
    nodeDump.open("svfg_nodes.csv");
    std::ofstream edgeDump;
    edgeDump.open("svfg_edges.csv");
    for(auto pair : *svfg)
    {
        auto id = pair.first;
        auto node = pair.second;
        nodeDump 
            << id 
            << "," << nodeKindToString(node->getNodeKind()) 
            << ",'" << node->getValue() << "'" << "\n";

        for(auto edge : node->getOutEdges())
        {
            edgeDump
                << edge->getEdgeKind()
                << "," << id 
                << "," << edge->getDstID() 
                << "\n";
        }
    }
    nodeDump.close();
    edgeDump.close();

    // clean up memory
    AndersenWaveDiff::releaseAndersenWaveDiff();
    SVFIR::releaseSVFIR();

    LLVMModuleSet::getLLVMModuleSet()->dumpModulesToFile(".svf.bc");
    SVF::LLVMModuleSet::releaseLLVMModuleSet();

    llvm::llvm_shutdown();
    return 0;
}

