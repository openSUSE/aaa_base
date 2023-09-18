#                                                                               
#    /etc/profile.d/alljava.sh                                                    
#                                                                               
# send feedback to http://bugs.opensuse.org

#
# This script sets some environment variables for default java.
# Affected variables: JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

if test -L /etc/alternatives/java
then
    ALTERNATIVES_JAVA_LINK=`realpath /etc/alternatives/java`
    ALTERNATIVES_JAVA_HOME=${ALTERNATIVES_JAVA_LINK/\/bin\/java}
fi

for JDIR in $ALTERNATIVES_JAVA_HOME /usr/lib64/jvm /usr/lib/jvm /usr/java/latest /usr/java; do

    if ! test -d $JDIR; then
        continue
    fi

    for JPATH in $JDIR $JDIR/*; do

    	if ! test -d $JPATH; then
            continue
    	fi

        if ! test -x $JPATH/bin/java; then
            continue
        fi

        export JAVA_BINDIR=$JPATH/bin
        export JAVA_ROOT=$JPATH
        export JAVA_HOME=$JPATH
        unset JDK_HOME
        unset SDK_HOME

        case "$JPATH" in
            *jre*)
                [ -z "$JRE_HOME" ] && export JRE_HOME=$JPATH
                ;;

            *)
		if [ -x $JPATH/jre/bin/java ] ; then
                    [ -z "$JRE_HOME" ] && export JRE_HOME=$JPATH/jre
		else
		    [ -z "$JRE_HOME" ] && export JRE_HOME=$JPATH
		fi
                # it is development kit
                if [ -x $JPATH/bin/javac ] ; then
                    export JDK_HOME=$JPATH
                    export SDK_HOME=$JPATH
                    unset JPATH
                    break 2; # we found a JRE + SDK -- don't look any further
                fi
                ;;
        esac

    done
    unset JPATH

done
unset JDIR
