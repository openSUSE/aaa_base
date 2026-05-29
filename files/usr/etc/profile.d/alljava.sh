#                                                                               
#    /etc/profile.d/alljava.sh                                                    
#                                                                               
# send feedback to http://bugs.opensuse.org

#
# This script sets some environment variables for default java.
# Affected variables: JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

if command -v alts >/dev/null 2>&1; then
    if alts -t java >/dev/null 2>&1; then
        JRE_HOME=$(alts -t java)
        export JRE_HOME=${JRE_HOME%/bin/java}
    fi

    if alts -t javac >/dev/null 2>&1; then
        JAVA_HOME=$(alts -t javac)
        export JAVA_HOME=${JAVA_HOME%/bin/javac}
    fi
fi

if [ -z "$JRE_HOME" ]; then
    if test -L /etc/alternatives/java -a -e /etc/alternatives/java; then
        ALTERNATIVES_JAVA_LINK=$(realpath /etc/alternatives/java 2>/dev/null)
        export JRE_HOME=${ALTERNATIVES_JAVA_LINK%/bin/java}
        unset ALTERNATIVES_JAVA_LINK
    fi
fi

if [ -z "$JAVA_HOME" ]; then
    if test -L /etc/alternatives/javac -a -e /etc/alternatives/javac; then
        ALTERNATIVES_JAVAC_LINK=$(realpath /etc/alternatives/javac 2>/dev/null)
        export JAVA_HOME=${ALTERNATIVES_JAVAC_LINK%/bin/javac}
        unset ALTERNATIVES_JAVAC_LINK
    fi
fi

if [ ! -z "$JAVA_HOME" ]; then
    export JAVA_BINDIR="$JAVA_HOME/bin"
    export JDK_HOME="$JAVA_HOME"
    export SDK_HOME="$JAVA_HOME"
fi
