iniDIR=$(pwd)
aimDIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
binDIR="$HOME/.bin"
command="pedant"
commandCompletion="profile.d/pedant_completion.sh"
# commandGui="pedant-gui"

if [ ! -d $binDIR ];then
	mkdir $binDIR
	echo "export PATH=\$PATH\":$binDIR\"" >> "$HOME/.bashrc"
fi
if [ ! -d "$binDIR/profile.d" ];then
	mkdir "$binDIR/profile.d"
	echo "source $binDIR/profile.d/*.sh" >> "$HOME/.bashrc"
fi
cd $binDIR
cat > "$command" << EOF
#!/bin/bash
cd "$aimDIR"
echo "Checking for updates ..."
git pull origin
./pedant \$@
if [[ \$? == 233 ]];then
	git add *.pdt
	git commit -m "conventional update"
	git push origin
fi
EOF
chmod +x "$command"
cat > "$commandCompletion" << EOF
function _pedant() {
    COMPREPLY=()
    if [[ \$COMP_CWORD == 1 ]] ; then
        COMPREPLY=( \$(compgen -W "start stop view log add undo" -- \${COMP_WORDS[COMP_CWORD]}) )
        return 0
    elif [[ \$COMP_CWORD == 2 ]] ; then
        local tasks=\$(ls $aimDIR/*.pdt | sed "s/\S*\///" | sed "s/.pdt//")
        COMPREPLY=( \$(compgen -W "\$tasks" -- \${COMP_WORDS[COMP_CWORD]}) )
        return 0
    fi
}
complete -F _pedant pedant
EOF
chmod +x "$commandCompletion"
. "$HOME/.bashrc"
# cat > "$commandGui" << EOF
# #!/bin/bash
# cd "$aimDIR"
# git pull origin
# 
# lang=\$1
# OPTION=\$(whiptail --nocancel --menu "Menu" 0 0 0 "1" "Start Studying" "2" "Stop Studying" "3" "View Report" "4" "View Log" 3>&1 1>&2 2>&3)
# if [ "\$OPTION" == 1 ]; then
# 	out=\$(./pedant start \$lang)
# 	ret=\$?
# 	whiptail --title "Start" --msgbox "\$out" 0 0
# elif [ "\$OPTION" == 2 ]; then
# 	out=\$(./pedant stop \$lang)
# 	ret=\$?
# 	whiptail --title "Stop" --msgbox "\$out" 0 0
# elif [ "\$OPTION" == 3 ]; then
# 	out=\$(./pedant view \$lang)
# 	ret=0
# 	whiptail --title "View" --scrolltext --msgbox "\$out" 0 0
# elif [ "\$OPTION" == 4 ]; then
# 	out=\$(./pedant log \$lang)
# 	ret=0
# 	whiptail --title "Log" --scrolltext --msgbox "\$out" 0 0
# else
# 	echo "\$OPTION"
# 	exit
# fi
# 
# if [ \$ret == 233 ]; then
# 	git add pedant.pdt
# 	git commit -m "conventional update"
# 	git push origin
# fi
# EOF
# chmod +x "$commandGui"

cd $aimDIR
g++ -std=c++11 pedant.cpp -o pedant

cd $iniDIR
