#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace llvm;
static llvm::cl::OptionCategory MyToolCategory("dump-file-ast options");

class DumpFileASTVisitor
    : public RecursiveASTVisitor<DumpFileASTVisitor> {
public:
    explicit DumpFileASTVisitor(ASTContext *Context, SourceManager &SrcMgr)
        : Context(Context), SrcMgr(SrcMgr) {}
    bool VisitDecl(Decl *d)
    {
        std::cout << "Scope in function" << d->isDefinedOutsideFunctionOrMethod() << std::endl;
        if (SrcMgr.isWrittenInMainFile(d->getLocation()))
            d->dumpColor();
        return true;
    }
private:
    ASTContext *Context;
    SourceManager &SrcMgr;
};

class DumpFileASTConsumer : public clang::ASTConsumer {
public:
    explicit DumpFileASTConsumer(ASTContext *Context, SourceManager &SrcMgr)
        : Visitor(Context, SrcMgr) {}
    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        // signature is RecursiveASTVisitor::TraverseAST(ASTContext &);
        Visitor.TraverseAST(Context);
    }
private:
   DumpFileASTVisitor Visitor; 
};

/// Builds a the AST consumer 
class DumpFileASTAction: public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer (
        clang::CompilerInstance &Compiler, llvm::StringRef InFile) 
    {
        return std::make_unique<DumpFileASTConsumer>(
            &Compiler.getASTContext(), Compiler.getSourceManager()
        );
    }
};

int main(int argc, const char **argv) {
  auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  clang::tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();
  clang:tooling::ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  return Tool.run(clang::tooling::newFrontendActionFactory<DumpFileASTAction>().get());
}
