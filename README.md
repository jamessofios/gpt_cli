# _gpt_cli_
## _The most terminal friendly ChatGPT application_

_gpt_cli_ is a flexible, easily scriptable ChatGPT interface for the UNIX/Linux terminal.

- Type your question
- Get an answer right away without ever having to leave your terminal
- ✨Magic ✨

## Features

- Ability to change settings such as LLM model selection (gpt-3.5-turbo, gpt-4, etc.)
- Export chat history as OpenAI API compatible JSON
- All data is stored either as plain text or as JSON

> The overriding design goal for gpt_cli is to be as friendly to the UNIX/Linux shell environment as possible.

## Tech

_gpt_cli_ uses a couple of open source libraries to work properly:

- `libcurl`
- `libjson-c`

And of course _gpt_cli_ requires use of OpenAI's ChatGPT API to function

## Installation

- First install the dependencies using the appropriate package manager for your system
- The dependencies are `libcurl` and `libjson-c`. In Debian and Ubuntu run `sudo apt install libcurl4 libcurl4-gnutls-dev` and `sudo apt install libjson-c5 libjson-c-dev` to install the required dependencies
- Next use the `make` command in this git repository folder to build the application

## How to use
Send a basic prompt to ChatGPT with default settings where you are not saving the chat history
```sh
gpt -u 'This is my first prompt'
```
Send a prompt to ChatGPT with default settings where you are saving the chat history
```sh
gpt -u 'This is my first prompt where I am saving the chat history' -j 'my_chat.json'
```
Send a prompt to ChatGPT where you change the model to gpt-4 and set the temperature to 0 with a system prompt
```sh
gpt -u 'Hello!' -s 'You are a dog' -m 'gpt-4' -t '0'
```
The command's options are listed below:
```sh
gpt [-m --model] [-t --temperature] [-s --system_prompt] [-u --user_prompt] [-j --json_file]
```
