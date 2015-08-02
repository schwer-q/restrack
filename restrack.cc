/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

//#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdio>

#include "restrack.hh"

#if 0
namespace {

	class PrintFunctionsConsumer : public ASTConsumer {
	public:
		virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
			for (DeclGroupRef::iterator it = DG.begin(), e = DG.end(); it != e; ++it) {
				const Decl *D = *it;
				if (const NamedDecl *ND = dyn_cast<NamedDecl>(D)) {
					std::string name = ND->getNameAsString();
					if (name == "malloc")
						llvm::errs() << "top-level-decl: \"" << name << "\"\n";
				}
			}
			return (true);
		}
	};

	class PrintFunctionNamesAction : public PluginASTAction {
	protected:
		std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
							       llvm::StringRef) {
			return (llvm::make_unique<PrintFunctionsConsumer>());
		}

		bool ParseArgs(const CompilerInstance &CI,
			       const std::vector<std::string>& args) {
			for (unsigned i = 0, e = args.size(); i != e; ++i) {
				llvm::errs() << "PrintFunctionNames arg = " << args[i] << "\n";

				if (args[i] == "-an-error") {
					DiagnosticsEngine &D = CI.getDiagnostics();
					unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
									    "invalid argument '%0'");
					D.Report(DiagID) << args[i];
					return (false);
				}
			}
			if (args.size() && args[0] == "help")
				PrintHelp(llvm::errs());

			return (true);
		}

		void PrintHelp(llvm::raw_ostream& ros) {
			ros << "Help for PrintFunctionNames plugin goes here\n";
		}
	};
}
#endif	// 0

RessourceTrackerVisitor::RessourceTrackerVisitor(clang::ASTContext *Context)
{
	this->Context = Context;
}

#if 0

bool
RessourceTrackerVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration)
{
	if (Declaration->getQualifiedNameAsString() == "n::m::C") {
		clang::FullSourceLoc FullLocation =
			this->Context->getFullLoc(Declaration->getLocStart());
		if (FullLocation.isValid()) {
			// llvm::outs() << "Found declaration at "
			// 	     << FullLocation.getSpellingLineNumber() << ":"
			// 	     << FullLocation.getSpellingColumnNumber() << "\n";
			Declaration->dump();
		}
	}
	return (true);
}

#endif	// 0

bool
RessourceTrackerVisitor::VisitBinaryOperator(clang::BinaryOperator *Operator)
{
	Operator->dump();
	return (true);
}

bool
RessourceTrackerVisitor::VisitCompoundStmt(clang::CompoundStmt *Expr)
{
	clang::ParentMap PM = clang::ParentMap(this->Context->getTranslationUnitDecl());
	return (true);
}

bool
RessourceTrackerVisitor::VisitDeclRefExpr(clang::DeclRefExpr *Reference)
{
	clang::ValueDecl *Declaration = Reference->getDecl();
	std::string name = Declaration->getNameAsString();

	llvm::outs() << BPINK << "DeclRefExpr" << RESET;

	if (clang::FunctionDecl::classof(Declaration)) {
		llvm::outs() << BGREEN << " Function" << RESET
			     << BCYAN << " '" << name << "'" << RESET;

		if (name == "calloc" || name == "malloc" ||
		    name == "mmap" || name == "realloc")
			llvm::outs() << RED << " memory allocator" << RESET;
		else if (name == "fopen" || name == "open" || name == "socket")
			llvm::outs() << RED << " fd allocator" << RESET;
		else if (name == "free" || name == "munmap")
			llvm::outs() << RED << " memory deallocator" << RESET;
		else if (name == "close" || name == "fclose")
			llvm::outs() << RED << " fd deallocator" << RESET;
	}

	if (clang::VarDecl::classof(Declaration)) {
		llvm::outs() << BGREEN << " Var" << RESET
			     << BCYAN << " '" << name << "'" << RESET;
	}

	llvm::outs() << "\n";

	return (true);
}

bool
RessourceTrackerVisitor::VisitExpr(clang::Expr *Expression)
{
	(void)Expression;
	return (true);
}

bool
RessourceTrackerVisitor::VisitParmVarDecl(clang::ParmVarDecl *Declaration)
{
	std::string varName = Declaration->getNameAsString();

	if (varName.length())
		llvm::outs() << "VisitParmVarDecl: <" << varName << ">\n";
	return (true);
}

bool
RessourceTrackerVisitor::VisitVarDecl(clang::VarDecl *Declaration)
{
	std::string varName = Declaration->getNameAsString();
	clang::QualType varType = Declaration->getType();
	clang::StorageClass SC = Declaration->getStorageClass();

	if (SC != clang::SC_None)
		llvm::outs() << clang::VarDecl::getStorageClassSpecifierString(SC);

	if (clang::ParmVarDecl::classof(Declaration))
		return (true);

	llvm::outs() << BGREEN << "VarDecl" << RESET
		     << BCYAN << " " << varName << RESET
		     << GREEN << " '" << clang::QualType::getAsString(varType.split()) << "'" << RESET;
	if (Declaration->hasInit()) {
		switch (Declaration->getInitStyle()) {
		case clang::VarDecl::CInit: llvm::outs() << " cinit"; break;
		case clang::VarDecl::CallInit: llvm::outs() << " callinit"; break;
		case clang::VarDecl::ListInit: llvm::outs() << " listinit"; break;
		}
	}
	llvm::outs() << "\n";
	return (true);
}

RessourceTrackerConsumer::RessourceTrackerConsumer(clang::ASTContext *Context) :
	Visitor(Context)
{

}

void
RessourceTrackerConsumer::HandleTranslationUnit(clang::ASTContext &Context)
{
	this->Visitor.TraverseDecl(Context.getTranslationUnitDecl());
}

std::unique_ptr<clang::ASTConsumer>
RessourceTrackerAction::CreateASTConsumer(clang::CompilerInstance &Compiler,
					  llvm::StringRef Infile)
{
	return (std::unique_ptr<clang::ASTConsumer>(
			new RessourceTrackerConsumer(&Compiler.getASTContext())));
}

bool
RessourceTrackerAction::ParseArgs(const clang::CompilerInstance &Compiler,
				  const std::vector<std::string> &args)
{
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		// blah blah blah...
	}
	return (true);
}

static clang::FrontendPluginRegistry::Add<RessourceTrackerAction>
X("restrack", "track ressources for potential leaks");
