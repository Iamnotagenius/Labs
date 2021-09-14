#!/bin/sh


ANSWERS=(13 83 3 2 13 57 0 1 0 0 0 0 5 45 8 5)
i=0
for testfile in tests/test?.txt; do
	words=$(../bin/wordcount -w $testfile)
	bytes=$(../bin/wordcount -c $testfile)
	lines=$(../bin/wordcount -l $testfile)
	not_empty=$(../bin/wordcount -L $testfile)
	if [[ $words -eq ${ANSWERS[$i]} && 
		  $bytes -eq ${ANSWERS[$(($i + 1))]} &&
		  $lines -eq ${ANSWERS[$(($i + 2))]} &&
		  $not_empty -eq ${ANSWERS[$(($i + 3))]} ]]; then
			echo -e "Test \e[33;1m$(($i / 4 + 1 )) \e[32mpassed\e[0m "
	else 
		echo -ne "Test \e[33;1m$(($i / 4 + 1)) \e[31mFAILED\E[0m ["
			if [[ $words -ne ${ANSWERS[$i]} ]]; then
				echo -ne "Words got: $words, Expected: ${ANSWERS[$i]}; "
			fi
			if [[ $bytes -ne ${ANSWERS[$(($i + 1))]} ]]; then
				echo -ne "Bytes got: $bytes, Expected: ${ANSWERS[$(($i + 1))]}; "
			fi
			if [[ $lines -ne ${ANSWERS[$(($i + 2))]} ]]; then
				echo -ne "Lines got: $lines, Expected: ${ANSWERS[$(($i + 2))]}; "
			fi
			if [[ $not_empty -ne ${ANSWERS[$(($i + 3))]} ]]; then
				echo -ne "Not empty lines got: $not_empty, Expected: ${ANSWERS[$(($i + 3))]}; "
			fi
		echo -e "\b\b  \b\b]"
	fi
	i=$(($i + 4))
done
