#                                                                               
#    /etc/profile.d/alljava.sh                                                    
#                                                                               
# send feedback to feedback@suse.de 

#
# This script sets some environment variables for default java.
# Affected variables: PATH, JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

if [ -e /usr/lib/java/bin/java ] || [ -e /usr/lib/java/bin/jre ] ; then
  export PATH=$PATH:/usr/lib/java/bin
  export JAVA_BINDIR=/usr/lib/java/bin
  export JAVA_HOME=/usr/lib/java
  if [ -d /usr/lib/java/jre/bin ] ; then
    export JRE_HOME=/usr/lib/java/jre
  else
    export JRE_HOME=/usr/lib/java
  fi        
  unset JDK_HOME
  unset SDK_HOME
  if [ -e /usr/lib/java/bin/javac ] ; then
    # it is development kit 
    if [ -e /usr/lib/java/bin/jre ] ; then
      export JDK_HOME=/usr/lib/java
    else
      export JDK_HOME=/usr/lib/java
      export SDK_HOME=/usr/lib/java
    fi
  fi
else
  if [ -d /usr/lib/java/jre/bin ] ; then
    # it is IBMJava2-JRE or SunJava2-JRE
    export PATH=$PATH:/usr/lib/java/jre/bin
    export JAVA_BINDIR=/usr/lib/java/jre/bin
    export JAVA_HOME=/usr/lib/java/jre
    export JRE_HOME=/usr/lib/java/jre
    unset JDK_HOME
    unset SDK_HOME
  fi
fi    

