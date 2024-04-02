#
#    /etc/profile.d/alljava.csh
#
# send feedback to http://bugs.opensuse.org

#
# This script sets some environment variables for default java.
# Affected variables: JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

if ( -l /etc/alternatives/java && -e /etc/alternatives/java ) then
    set ALTERNATIVES_JAVA_LINK=`realpath /etc/alternatives/java`
    setenv JRE_HOME $ALTERNATIVES_JAVA_LINK:h:h
    unset ALTERNATIVES_JAVA_LINK
endif

if ( -l /etc/alternatives/javac && -e /etc/alternatives/javac )  then
    set ALTERNATIVES_JAVAC_LINK=`realpath /etc/alternatives/javac`
    setenv JAVA_HOME $ALTERNATIVES_JAVAC_LINK:h:h
    setenv JAVA_BINDIR $JAVA_HOME/bin
    setenv JDK_HOME $JAVA_HOME
    setenv SDK_HOME $JAVA_HOME
    unset ALTERNATIVES_JAVAC_LINK
endif

