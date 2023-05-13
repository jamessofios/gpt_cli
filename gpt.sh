#!/bin/sh
# Author: James Sofios
# Date: 05/10/2023
# Dependancies: curl jq
# Please set the environment variable $OPENAI_API_KEY accordingly.
#
# Reference: OpenAI API
# https://platform.openai.com/docs/api-reference/making-requests
#

# TODO: Prepromp, pinecone

call_gpt()
{
	system_prompt="$1"
	user_prompt="$2"
	temp="$3"

	curl https://api.openai.com/v1/chat/completions \
		-H "Content-Type: application/json" \
		-H "Authorization: Bearer $OPENAI_API_KEY" \
		-d "{
			\"model\": \"gpt-3.5-turbo\",
			\"messages\": [
				{\"role\": \"system\", \"content\": \""$system_prompt"\"},
				{\"role\": \"user\", \"content\": \""$user_prompt"\"}
				],
			\"temperature\": $temp
		}" 2> /dev/null | jq -r '.choices[0].message.content'
}


if [ -p /dev/stdin ]
then

	prompt=''

	while read -r line
	do
		if [ "$prompt" ]
		then
			prompt=$(printf "%s\n%s" "$prompt" "$line")
		else
			prompt=$(printf "%s\n" "$line")
		fi
	done
	prompt=$(printf "%s" "$prompt" | sed "s/'/'\\\\''/g" | sed 's/"/\\"/g')
#	printf "%s\n" "$prompt"
	call_gpt "$prompt" "$prompt" 1
else
	call_gpt "$1" "$2" "$3"
fi
