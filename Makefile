# -*- MakeFile -*-
CC=/usr/bin/clang # clang-13
C_VERSION=-std=c17
GNU_SOURCE_SOCKETS=-D_GNU_SOURCE
# https://clang.llvm.org/docs/DiagnosticsReference.html
C_FLAGS=-pedantic -Wall -Wextra -Walloca -Wanon-enum-enum-conversion -Warc-maybe-repeated-use-of-weak \
	-Warray-bounds-pointer-arithmetic -Warray-parameter -Wassign-enum -Watomic-implicit-seq-cst -Wbad-function-cast \
	-Wbitwise-instead-of-logical -Wbitwise-op-parentheses -Wbool-operation -Wc11-extensions -Wc2x-compat \
	-Wc2x-extensions -Wc2x-extensions -Wcast-align -Wcast-function-type -Wcast-function-type-strict -Wcast-qual \
	-Wcast-qual -Wchar-subscripts -Wcomma -Wcomment -Wcompletion-handler -Wcompound-token-split-by-space \
	-Wconditional-uninitialized -Wconsumed -Wconversion -Wconversion-null -Wcovered-switch-default \
	-Wcstring-format-directive -Wcustom-atomic-properties -Wdate-time -Wdeprecated -Wdocumentation \
	-Wdocumentation-deprecated-sync -Wdocumentation-pedantic -Wdocumentation-unknown-command \
	-Wdollar-in-identifier-extension -Wdouble-promotion -Wduplicate-decl-specifier -Wduplicate-enum \
	-Wduplicate-method-arg -Wduplicate-method-match -Wembedded-directive -Wempty-init-stmt -Wempty-translation-unit \
	-Wenum-compare-conditional -Wenum-conversion -Wenum-enum-conversion -Wenum-float-conversion -Wexpansion-to-defined \
	-Wexplicit-ownership-type -Wflexible-array-extensions -Wfloat-conversion -Wfloat-equal -Wfloat-overflow-conversion \
	-Wfloat-zero-conversion -Wfor-loop-analysis -Wformat-nonliteral -Wformat-pedantic -Wformat-type-confusion \
	-Wframe-address -Widiomatic-parentheses -Wimplicit-atomic-properties -Wimplicit-fallthrough \
	-Wimplicit-fallthrough-per-function -Wimplicit-float-conversion -Wimplicit-function-declaration -Wimplicit-int \
	-Wimplicit-int-conversion -Wimplicit-int-float-conversion -Wimplicit-retain-self -Winfinite-recursion \
	-Wint-in-bool-context -Winvalid-or-nonexistent-directory -Wlogical-op-parentheses -Wmethod-signatures \
	-Wmisleading-indentation -Wmissing-braces -Wmissing-field-initializers -Wmissing-method-return-type \
	-Wmissing-noreturn -Wmissing-prototypes -Wmissing-variable-declarations -Wmost -Wnewline-eof \
	-Wnonportable-system-include-path -Wnull-pointer-arithmetic -Wnull-pointer-subtraction \
	-Wnullable-to-nonnull-conversion -Wobjc-signed-char-bool-implicit-int-conversion -Wover-aligned \
	-Woverriding-method-mismatch -Wparentheses -Wpedantic -Wpointer-arith -Wpoison-system-directories \
	-Wredundant-parens -Wreserved-macro-identifier -Wreturn-stack-address -Rsanitize-address -Wself-assign \
	-Wself-assign-overloaded -Wself-move -Wshift-sign-overflow -Wshorten-64-to-32 -Wsign-compare -Wsign-conversion \
	-Wsometimes-uninitialized -Wstrict-prototypes -Wstrict-selector-match -Wstring-concatenation -Wstring-conversion \
	-Wsuspicious-bzero -Wswitch-enum -Wswitch-enum -Wtautological-overlap-compare -Wtautological-type-limit-compare \
	-Wtautological-unsigned-char-zero-compare -Wtautological-unsigned-enum-zero-compare \
	-Wtautological-unsigned-zero-compare -Wtautological-value-range-compare -Wthread-safety -Wthread-safety-analysis \
	-Wthread-safety-attributes -Wthread-safety-beta -Wthread-safety-negative -Wthread-safety-precise \
	-Wthread-safety-reference -Wthread-safety-verbose -Wundef -Wundef-prefix -Wundefined-func-template -Wuninitialized \
	-Wuninitialized-const-reference -Wunneeded-internal-declaration -Wunneeded-member-function -Wunreachable-code \
	-Wunreachable-code-aggressive -Wunused -Wunused-const-variable -Wunused-function -Wunused-label \
	-Wunused-lambda-capture -Wunused-local-typedef -Wunused-macros -Wunused-member-function -Wunused-parameter \
	-Wunused-result -Wunused-variable -Wvexing-parse -Wzero-as-null-pointer-constant

# https://www.youtube.com/watch?v=DtGrdB8wQ_8&ab_channel=GabrielParmer
# https://github.com/gwu-cs-os/evening_os_hour/tree/master/f19/10.2-makefiles
# https://www.youtube.com/watch?v=eMfYyijl148&ab_channel=BhaskarChowdhury...MusingwithGNU_Linux%21%21
# https://www.youtube.com/watch?v=43-2t7CveRI&ab_channel=Dkeathly
# https://www.youtube.com/watch?v=GExnnTaBELk&ab_channel=BarryBrown

SRC_MAIN=src/
SRC_LAB_SOCKET=src/lab_socket/
C_FILES= main.c lab_socket.c

OBJECTS=bin/lab_socket.o
BINARY=PedroR_lab1

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) $(C_VERSION) $(C_FLAGS) $(GNU_SOURCE_SOCKETS) -c -o $@ $<

clean:
	rm -f $(BINARY) $(C_FILES)
