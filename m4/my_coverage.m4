# SYNOPSIS
#
#   MY_COVERAGE()
#
# DESCRIPTION
#
#   Defines MY_COVERAGE_CFLAGS, MY_COVERAGE_CXXFLAGS and MY_COVERAGE_LDFLAGS which should be
#   included in the CFLAGS, CXXFLAGS and LIBS/LDFLAGS variables of every build target
#   (program or library) which should be built with code coverage support.
#   Also defines MY_COVERAGE_RULES which should be substituted in your
#   Makefile.am; and $enable_code_coverage which can be used in subsequent
#   configure output. MY_COVERAGE_ENABLED is defined and substituted, and
#   corresponds to the value of the --enable-my-coverage option, which
#   defaults to being disabled.
#
#   Note that all optimisation flags in CFLAGS/CXXFLAGS defined before MY_COVERAGE execution
#   will be stripped when code coverage is enabled.
#
#   Usage example:
#   configure.ac:
#      MY_COVERAGE
#
#   Makefile.am:
#      @MY_COVERAGE_RULES@
#      my_program_LIBS = … $(MY_COVERAGE_LDFLAGS) …
#      my_program_CFLAGS = … $(MY_COVERAGE_CFLAGS) …
#      my_program_CXXFLAGS = … $(MY_COVERAGE_CXXFLAGS) …
#
#   This results in a “coverage” rule being added to any Makefile.am which includes “@MY_COVERAGE_RULES@”
#   (assuming the module has been configured with --enable-my-coverage). Running `make coverage`
#   in that directory generate a code coverage report detailing the code which was touched,
#   then print the URI for the report.
#
# CREDITS
#
#   MY_COVERAGE is mostly based on the AX_COVERAGE / GNOME_COVERAGE macro:
#
#       http://savannah.gnu.org/patch/?8451
#
#   with some bits from protobuf-c version:
#
#       https://github.com/protobuf-c/protobuf-c/blob/e72e7e7e81d75206f54918c5270e334bd2c25fd6/m4/code_coverage.m4
#
# LICENSE
#
#   Copyright © 2012, 2014 Philip Withnall
#   Copyright © 2012 Xan Lopez
#   Copyright © 2012 Christian Persch
#   Copyright © 2012 Paolo Borelli
#   Copyright © 2012 Dan Winship
#   Copyright © 2014 Robert Edmonds
#   Copyright © 2014 Oleg Efimov
#
#   This file is licenced under MIT license.

AC_DEFUN([MY_COVERAGE],
[
  dnl Check for --enable-my-coverage
  AC_MSG_CHECKING([whether to build with code coverage])
  AC_ARG_ENABLE(
    [my-coverage],
    AS_HELP_STRING([--enable-my-coverage], [enable code coverage]),
    enable_my_coverage=yes,
    enable_my_coverage=no
  )
  AC_MSG_RESULT($enable_my_coverage)

  AM_CONDITIONAL([MY_COVERAGE_ENABLED], [test "x$enable_my_coverage" = "xyes"])
  AC_SUBST([MY_COVERAGE_ENABLED], [$enable_my_coverage])

  dnl Do extra checks for enabled code coverage
  if test "x$enable_my_coverage" = "xyes"; then
    dnl We need gcc
    if test "x$GCC" != "xyes"; then
      AC_MSG_ERROR([GCC is required for --enable-my-coverage])
    fi

    dnl Check if ccache is being used
    AC_CHECK_PROG(SHTOOL, shtool, shtool)
    if test "$SHTOOL"; then
      AS_CASE(
        [`$SHTOOL path $CC`],
        [*ccache*],
        [ccache=yes],
        [ccache=no]
      )
    fi

    if test "x$ccache" = "xyes" && (test -z "$CCACHE_DISABLE" || test "$CCACHE_DISABLE" != "1"); then
      AC_MSG_ERROR([ccache must be disabled when --enable-my-coverage option is used. You can disable ccache by setting environment variable CCACHE_DISABLE=1.])
    fi

    AC_CHECK_PROG(GCOV, gcov, gcov)
    if test -z "$GCOV"; then
      AC_MSG_ERROR([Could not find gcov from the gcc package])
    fi

    AC_CHECK_PROG(LCOV, lcov, lcov)
    if test -z "$LCOV"; then
      AC_MSG_ERROR([Could not find lcov from the lcov package])
    fi

    AC_CHECK_PROG(GENHTML, genhtml, genhtml)
    if test -z "$GENHTML"; then
      AC_MSG_ERROR([Could not find genhtml from the lcov package])
    fi

    dnl Remove all optimization flags from compiler flags
    changequote({,})
    CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9]*//g'`
    CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9]*//g'`
    changequote([,])

    dnl Add the special compiler flags
    MY_COVERAGE_CFLAGS="--coverage -g -O0"
    MY_COVERAGE_CXXFLAGS="--coverage -g -O0"
    MY_COVERAGE_LDFLAGS="--coverage"
  fi

  AC_SUBST(MY_COVERAGE_CFLAGS)
  AC_SUBST(MY_COVERAGE_CXXFLAGS)
  AC_SUBST(MY_COVERAGE_LDFLAGS)

  MY_COVERAGE_RULES='
# {{{ MY_COVERAGE_RULES
#
# Code coverage
#
# Optional:
#  - MY_COVERAGE_DIRECTORY: Top-level directory for code coverage reporting.
#    (Default: $(abs_top_builddir))
#  - MY_COVERAGE_OUTPUT_FILE: Filename and path for the .info file generated
#    by lcov for code coverage. (Default:
#    $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage.info)
#  - MY_COVERAGE_OUTPUT_DIRECTORY: Directory for generated code coverage
#    reports to be created. (Default:
#    $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage)
#  - MY_COVERAGE_LCOV_OPTIONS: Extra options to pass to the lcov instance.
#    (Default: empty)
#  - MY_COVERAGE_GENHTML_OPTIONS: Extra options to pass to the genhtml
#    instance. (Default: empty)
#  - MY_COVERAGE_IGNORE_PATTERN: Extra glob pattern of files to ignore.
#    (Default: empty)
#
# The generated report will be titled using the $(PACKAGE_NAME) and
# $(PACKAGE_VERSION).

# Optional variables
MY_COVERAGE_DIRECTORY ?= $(abs_top_builddir)
MY_COVERAGE_OUTPUT_FILE ?= $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage.info
MY_COVERAGE_OUTPUT_DIRECTORY ?= $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage
MY_COVERAGE_LCOV_OPTIONS ?=
MY_COVERAGE_GENHTML_OPTIONS ?=
MY_COVERAGE_IGNORE_PATTERN ?=

# Some additional lcov options
MY_COVERAGE_LCOV_OPTIONS += --no-external --no-checksum --compat-libtool

MY_V_COV = $(my_v_COV_$(V))
my_v_COV_ = $(my_v_COV_$(AM_DEFAULT_VERBOSITY))
my_v_COV_0 = @echo "  COV     " $$@@;
my_v_COV_1 =

MY_COVERAGE_QUIET = $(my_coverage_quiet_$(V))
my_coverage_quiet_ = $(my_coverage_quiet_$(AM_DEFAULT_VERBOSITY))
my_coverage_quiet_0 = --quiet
my_coverage_quiet_1 =

# Rules

clean-gcno:
	@echo Removing old coverage sources
	-find . -name '*.gcno' -delete

clean-gcda:
	@echo Removing old coverage results
	-find . -name '*.gcda' -delete

clean-gcov:
	@echo Removing old coverage sources
	-find . -name '*.gcov' -delete

.PHONY: clean-gcno clean-gcda clean-gcov

$(MY_COVERAGE_OUTPUT_FILE).tmp:
ifeq ($(MY_COVERAGE_ENABLED),yes)
	$(MY_V_COV)$(LCOV) $(MY_COVERAGE_QUIET) --directory $(MY_COVERAGE_DIRECTORY) --capture --output-file $(MY_COVERAGE_OUTPUT_FILE).tmp $(MY_COVERAGE_LCOV_OPTIONS)
else
	$(error "Need to reconfigure with --enable-my-coverage")
endif

$(MY_COVERAGE_OUTPUT_FILE): $(MY_COVERAGE_OUTPUT_FILE).tmp
ifeq ($(MY_COVERAGE_ENABLED),yes)
	$(MY_V_COV)$(LCOV) $(MY_COVERAGE_QUIET) --directory $(MY_COVERAGE_DIRECTORY) --remove $(MY_COVERAGE_OUTPUT_FILE).tmp $(MY_COVERAGE_IGNORE_PATTERN) --output-file $(MY_COVERAGE_OUTPUT_FILE)
	-@rm -f $(MY_COVERAGE_OUTPUT_FILE).tmp
else
	$(error "Need to reconfigure with --enable-my-coverage")
endif

$(MY_COVERAGE_OUTPUT_DIRECTORY): $(MY_COVERAGE_OUTPUT_FILE)
ifeq ($(MY_COVERAGE_ENABLED),yes)
	$(MY_V_COV)LANG=C $(GENHTML) $(MY_COVERAGE_QUIET) --prefix $(MY_COVERAGE_DIRECTORY) --output-directory $(MY_COVERAGE_OUTPUT_DIRECTORY) --title "$(PACKAGE_NAME)-$(PACKAGE_VERSION) Code Coverage" --legend --show-details $(MY_COVERAGE_OUTPUT_FILE) $(MY_COVERAGE_GENHTML_OPTIONS)
else
	$(error "Need to reconfigure with --enable-my-coverage")
endif

coverage: $(MY_COVERAGE_OUTPUT_DIRECTORY)
ifeq ($(MY_COVERAGE_ENABLED),yes)
	$(MY_V_COV)$(LCOV) --summary $(MY_COVERAGE_OUTPUT_FILE)
	@echo "file://$(abs_builddir)/$(MY_COVERAGE_OUTPUT_DIRECTORY)/index.html"
else
	$(error "Need to reconfigure with --enable-my-coverage")
endif

clean-coverage: clean-gcda clean-gcov
	$(LCOV) --directory $(MY_COVERAGE_DIRECTORY) -z
	rm -rf $(MY_COVERAGE_OUTPUT_FILE) $(MY_COVERAGE_OUTPUT_FILE).tmp $(MY_COVERAGE_OUTPUT_DIRECTORY)

.PHONY: coverage clean-coverage

clean-local: clean-coverage

distclean-local: clean-gcno

# }}} MY_COVERAGE_RULES
'

  AC_SUBST([MY_COVERAGE_RULES])
  m4_ifdef([_AM_SUBST_NOTMAKE], [_AM_SUBST_NOTMAKE([MY_COVERAGE_RULES])])

]) # MY_COVERAGE

