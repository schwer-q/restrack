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

#ifndef __RESTRACK_HH
#define __RESTRACK_HH

#define	RESET	"\033[00m"
#define	BOLD	"\033[01m"
#define	BLACK	"\033[30m"
#define	RED	"\033[31m"
#define	GREEN	"\033[32m"
#define	YELLOW	"\033[33m"
#define	BLUE	"\033[34m"
#define	PINK	"\033[35m"
#define	CYAN	"\033[36m"
#define	WHITE	"\033[37m"
#define	BBLACK	"\033[01;30m"
#define	BRED	"\033[01;31m"
#define	BGREEN	"\033[01;32m"
#define	BYELLOW	"\033[01;33m"
#define	BBLUE	"\033[01;34m"
#define	BPINK	"\033[01;35m"
#define	BCYAN	"\033[01;36m"
#define	BWHITE	"\033[01;37m"

namespace {
	class RessourceTrackerFunction;
	class RessourceTrackerScope;
	class RessourceTrackerVariable;
	class RessourceTrackerVisitor;
	class RessourceTrackerConsumer;
	class RessourceTrackerAction;

	/*
	 * Represent a function within the AST
	 * We hold the top level scope for the function.
	 */
	class RessourceTrackerFunction {
	private:
		clang::FunctionDecl	*m_Function;
		// std::list<RessourceTrackerVariable> m_parmvar;
		// a FunctionDecl create a new scope even if there is
		// a CompoundStmt right after, so we keep ParmVarDecl
		// as variables in the top level scope for the function
		RessourceTrackerScope	*m_Scope;

	public:
		RessourceTrackerFunction(clang::FunctionDecl *);
	};

	/*
	 * Represent a scope within the AST
	 * A scope hold a pointer to his parent and store a list
	 * of scopes that are at a level scope+1
	 */
	class RessourceTrackerScope {
	private:
		std::list<RessourceTrackerVariable *>	m_Variables;
		RessourceTrackerScope			*m_Parent;
		std::list<RessourceTrackerScope *>	m_Childs;

	public:
		RessourceTrackerScope(RessourceTrackerScope *);
	};

	/*
	 * Represent a variable whithin the AST
	 */
	class RessourceTrackerVariable {
	private:
		clang::VarDecl	*m_Variable;
		bool		m_ParmVar; // XXX: Usefull?

		bool		m_Ressource; // the variable is a ressource
		bool		m_Returned;  // the variable is returned

	public:
		RessourceTrackerVariable(clang::VarDecl *);

		std::string	getName(void) const;
		bool		isRessource(void) const;
		bool		isReturned(void) const;

		void		setRessource(bool);
		void		setReturned(bool);
	};

	class RessourceTrackerVisitor :
		public clang::RecursiveASTVisitor<RessourceTrackerVisitor> {
	private:
		// clang::ASTContext	*m_Context;
		// clang::ASTContext *Context __attribute((deprecated))__;
		clang::ASTContext *Context;
		clang::ParentMap *ParentMap; // XXX: kill it

		// RessourceTrackerScope	*m_GlobalScope;
		std::list<RessourceTrackerFunction> m_Functions;

	public:
		explicit RessourceTrackerVisitor(void);
		~RessourceTrackerVisitor(void);

		void setContext(clang::ASTContext &);

		bool VisitBinaryOperator(clang::BinaryOperator *);
		bool VisitCompoundStmt(clang::CompoundStmt *);
		bool VisitDeclRefExpr(clang::DeclRefExpr *);
		bool VisitExpr(clang::Expr *);
		bool VisitFunctionDecl(clang::FunctionDecl *);
		bool VisitParmVarDecl(clang::ParmVarDecl *);
		bool VisitVarDecl(clang::VarDecl *);
	};

	class RessourceTrackerConsumer : public clang::ASTConsumer {
	private:
		RessourceTrackerVisitor Visitor;

	public:
		explicit RessourceTrackerConsumer(clang::ASTContext *);

		virtual void HandleTranslationUnit(clang::ASTContext&);
	};

	class RessourceTrackerAction : public clang::PluginASTAction {
	public:
		virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
			clang::CompilerInstance&, llvm::StringRef);
		bool ParseArgs(const clang::CompilerInstance&,
			       const std::vector<std::string>&);
	};
}

#endif
