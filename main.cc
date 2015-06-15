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

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

#include "restrack.hh"

using namespace clang;

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

std::unique_ptr<clang::ASTConsumer>
RessourceTrackerAction::CreateASTConsumer(clang::CompilerInstance &Compiler,
					  llvm::StringRef Infile)
{
	return (std::unique_ptr<clang::ASTConsumer>(
			new RessourceTrackerConsumer));
}

bool
RessourceTrackerAction::ParseArgs(const CompilerInstance &Compiler,
				  const std::vector<std::string> &args)
{
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		// blah blah blah...
	}
	return (true);
}

static FrontendPluginRegistry::Add<RessourceTrackerAction>
X("restrack", "track ressources for potential leaks");