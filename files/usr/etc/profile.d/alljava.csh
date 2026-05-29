#
#    /etc/profile.d/alljava.csh
#
# send feedback to http://bugs.opensuse.org

#
# This script sets some environment variables for default java.
# Affected variables: JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

which alts >& /dev/null
if ( $status == 0 ) then
    alts -t java >& /dev/null
    if ( $status == 0 ) then
        set JAVA_TARGET `alts -t java`
    endif

    alts -t javac >& /dev/null
    if ( $status == 0 ) then
        set JAVAC_TARGET `alts -t javac`
    endif
endif

if ( ! $?JAVA_TARGET ) then
    if ( -l /etc/alternatives/java && -e /etc/alternatives/java ) then
        set JAVA_TARGET=`realpath /etc/alternatives/java`
    endif
endif

if ( ! $?JAVAC_TARGET ) then
    if ( -l /etc/alternatives/javac && -e /etc/alternatives/javac ) then
        set JAVAC_TARGET=`realpath /etc/alternatives/javac`
    endif
endif

if ( $?JAVA_TARGET ) then
    setenv JRE_HOME $JAVA_TARGET:h:h
    unset JAVA_TARGET
endif

if ( $?JAVAC_TARGET ) then
    setenv JAVA_HOME $JAVAC_TARGET:h:h
    setenv JAVA_BINDIR $JAVAC_TARGET:h
    setenv JDK_HOME $JAVA_HOME
    setenv SDK_HOME $JAVA_HOME
    unset JAVAC_TARGET
endif
