#                                                                               
#    /etc/profile.d/alljava.sh                                                    
#                                                                               
# send feedback to http://bugs.opensuse.org

#
# This script sets some environment variables for default java.
# Affected variables: JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

for JDIR in /usr/lib64/jvm /usr/lib/jvm /usr/java/latest /usr/java; do

    if ! test -d $JPATH; then
        continue
    fi

    for JPATH in $JDIR $JDIR/java `ls -I 'java' -I 'jre' -d $JDIR/* 2>/dev/null` $JDIR/jre; do

        if ! test -x $JPATH/bin/java ; then
            continue
        fi

        export JAVA_BINDIR=3D$JPATH/bin
        export JAVA_ROOT=3D$JPATH
        export JAVA_HOME=3D$JPATH
        unset JDK_HOME
        unset SDK_HOME

        case "$JPATH" in
            *jre*)
                export JRE_HOME=3D$JPATH
                ;;

            *)
                export JRE_HOME=3D$JPATH/jre
                # it is development kit
                if [ -x $JPATH/bin/javac ] ; then
                    export JDK_HOME=3D$JPATH
                    export SDK_HOME=3D$JPATH
                fi
                ;;
        esac

    done
    unset JPATH

done
unset JDIR
