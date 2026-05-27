#!/bin/sh
#
# to be placed in /usr/share/subversion/hook-scripts/commit-email.pl
# called from SVN hook: /var/svn/aaaDev/hooks/post-commit
# 

REPO=$1
REV=$2

TMPMSG=/tmp/svn-commit.$REV
SVNLOOK=/usr/bin/svnlook
NULLMAIL=/usr/bin/nullmailer-inject
MAILADDR=aaaseed-dev@binalog.com
MAILFROM=aaaseed-dev@binalog.com
AUTHOR=`$SVNLOOK author -r $REV $REPO`

#echo to:maa.berriet@gmail.com > $TMPMSG
#echo to:franz.hildgen@gmail.com >> $TMPMSG
#echo to:a.rebeko@binalog.com >> $TMPMSG

echo to:$MAILADDR   > $TMPMSG
echo from:$MAILFROM >> $TMPMSG
echo subject: SVN-commit Rev.$REV $AUTHOR [$REPO] >> $TMPMSG
echo "Content-Type: text/plain; charset=ISO-8859-1; format=flowed" >> $TMPMSG
echo Content-Transfer-Encoding: 7bit >> $TMPMSG
echo Rev.$REV $REPO  >> $TMPMSG
echo ======================================= >> $TMPMSG
$SVNLOOK info -r $REV $REPO >> $TMPMSG
echo ======================================= >> $TMPMSG
$SVNLOOK changed -r $REV $REPO >> $TMPMSG
echo ======================================= >> $TMPMSG
$NULLMAIL -f $MAILFROM < $TMPMSG
rm $TMPMSG


