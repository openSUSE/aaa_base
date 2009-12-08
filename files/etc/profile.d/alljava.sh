#                                                                               
#    /etc/profile.d/alljava.sh                                                    
#                                                                               
# send feedback to http://www.suse.de/feedback

#
# This script sets some environment variables for default java.
# Affected variables: PATH, JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

__libdir=lib
if [ -x /usr/lib64/jvm/java ] || [ -x /usr/lib64/jvm/jre ] ; then
  __libdir=lib64
fi

if [ -x /usr/$__libdir/jvm/java/bin/java ] || [ -x /usr/$__libdir/jvm/java/bin/jre ] ; then
  export JAVA_BINDIR=/usr/$__libdir/jvm/java/bin
  export JAVA_ROOT=/usr/$__libdir/jvm/java
  export JAVA_HOME=/usr/$__libdir/jvm/java
  export JRE_HOME=/usr/$__libdir/jvm/jre
  unset JDK_HOME
  unset SDK_HOME
  if [ -x /usr/$__libdir/jvm/java/bin/javac ] ; then
    # it is development kit 
    if [ -x /usr/$__libdir/jvm/java/bin/jre ] ; then
      export JDK_HOME=/usr/$__libdir/jvm/java
    else
      export JDK_HOME=/usr/$__libdir/jvm/java
      export SDK_HOME=/usr/$__libdir/jvm/java
    fi
  fi
else
  if [ -x /usr/$__libdir/jvm/jre/bin/java ] ; then
    # it is IBMJava2-JRE or SunJava2-JRE
    export PATH=$PATH:/usr/$__libdir/jvm/jre/bin
    export JAVA_BINDIR=/usr/$__libdir/jvm/jre/bin
    export JAVA_ROOT=/usr/$__libdir/jvm/jre
    export JAVA_HOME=/usr/$__libdir/jvm/jre
    export JRE_HOME=/usr/$__libdir/jvm/jre
    unset JDK_HOME
    unset SDK_HOME
  fi
fi    

unset __libdir
