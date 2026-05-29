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
        JAVA_TARGET=$(alts -t java)
    fi

    if alts -t javac >/dev/null 2>&1; then
        JAVAC_TARGET=$(alts -t javac)
    fi
fi

if [ -z "$JAVA_TARGET" ]; then
    if test -L /etc/alternatives/java -a -e /etc/alternatives/java; then
        JAVA_TARGET=$(realpath /etc/alternatives/java 2>/dev/null)
    fi
fi

if [ -z "$JAVAC_TARGET" ]; then
    if test -L /etc/alternatives/javac -a -e /etc/alternatives/javac; then
        JAVAC_TARGET=$(realpath /etc/alternatives/javac 2>/dev/null)
    fi
fi

if [ ! -z "$JAVA_TARGET" ]; then
    export JRE_HOME=${JRE_HOME%/bin/java}
    unset JAVA_TARGET
fi

if [ ! -z "$JAVAC_TARGET" ]; then
    export JAVA_HOME=${JAVAC_TARGET%/bin/javac}
    export JAVA_BINDIR=${JAVAC_TARGET%/javac}
    export JDK_HOME="$JAVA_HOME"
    export SDK_HOME="$JAVA_HOME"
    unset JAVAC_TARGET
fi
