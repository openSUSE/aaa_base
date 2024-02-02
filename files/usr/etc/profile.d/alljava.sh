#                                                                               
#    /etc/profile.d/alljava.sh                                                    
#                                                                               
# send feedback to http://bugs.opensuse.org

#
# This script sets some environment variables for default java.
# Affected variables: JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

if test -L /etc/alternatives/java -a -e /etc/alternatives/java; then
    ALTERNATIVES_JAVA_LINK=`realpath /etc/alternatives/java 2> /dev/null`
    export JRE_HOME=${ALTERNATIVES_JAVA_LINK%/bin/java}
    unset ALTERNATIVES_JAVA_LINK
fi

if test -L /etc/alternatives/javac -a -e /etc/alternatives/javac; then
    ALTERNATIVES_JAVAC_LINK=`realpath /etc/alternatives/javac 2> /dev/null`
    export JAVA_HOME=${ALTERNATIVES_JAVAC_LINK%/bin/javac}
    export JAVA_BINDIR=$JAVA_HOME/bin
    export JDK_HOME=$JAVA_HOME
    export SDK_HOME=$JAVA_HOME
    unset ALTERNATIVES_JAVAC_LINK
fi

