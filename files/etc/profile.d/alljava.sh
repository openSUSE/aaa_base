#                                                                               
#    /etc/profile.d/alljava.sh                                                    
#                                                                               
# send feedback to http://www.suse.de/feedback

#
# This script sets some environment variables for default java.
# Affected variables: PATH, JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

if [ -x /usr/lib/jvm/java/bin/java ] || [ -x /usr/lib/jvm/java/bin/jre ] ; then
  export JAVA_BINDIR=/usr/lib/jvm/java/bin
  export JAVA_ROOT=/usr/lib/jvm/java
  export JAVA_HOME=/usr/lib/jvm/java
  if [ -x /usr/lib/jvm/java/jre/bin/java ] ; then
    export JRE_HOME=/usr/lib/jvm/java/jre
  else
    export JRE_HOME=/usr/lib/jvm/java
  fi        
  unset JDK_HOME
  unset SDK_HOME
  if [ -x /usr/lib/jvm/java/bin/javac ] ; then
    # it is development kit 
    if [ -x /usr/lib/jvm/java/bin/jre ] ; then
      export JDK_HOME=/usr/lib/jvm/java
    else
      export JDK_HOME=/usr/lib/jvm/java
      export SDK_HOME=/usr/lib/jvm/java
    fi
  fi
else
  if [ -x /usr/lib/jvm/jre/bin/java ] ; then
    # it is IBMJava2-JRE or SunJava2-JRE
    export PATH=$PATH:/usr/lib/jvm/jre/bin
    export JAVA_BINDIR=/usr/lib/jvm/jre/bin
    export JAVA_ROOT=/usr/lib/jvm/jre
    export JAVA_HOME=/usr/lib/jvm/jre
    export JRE_HOME=/usr/lib/jvm/jre
    unset JDK_HOME
    unset SDK_HOME
  fi
fi    
