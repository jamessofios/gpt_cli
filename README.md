# _gpt_cli_
## _The most terminal friendly ChatGPT appication_

_gpt_cli_ is a flexible, easily scriptable ChatGPT interface for the UNIX/Linux terminal.

- Type your question
- Get an answer right away without ever having to leave your terminal
- ✨Magic ✨

## Features

- Ability to change settings such as LLM model selection (gpt-3.5-turbo, gpt-4, etc.)
- Export chat history as OpenAI API compatible JSON
- All data is stored either as plain text or as JSON

Markdown is a lightweight markup language based on the formatting conventions
that people naturally use in email.

> The overriding design goal for gpt_cli is to be as friendly to the UNIX/Linux shell environment as possible.

## Tech

_gpt_cli_ uses a couple of open source libraries to work properly:

- libcurl
- libjson-c

And of course _gpt_cli_ requires use of OpenAI's ChatGPT API to funtion

## Installation


- First install the dependancies using the appropriate package manager for your system
-- The dependancies are libcurl and libjson-c
- Next use the make command in this git repository folder to build the application

## How to use
Send a basic prompt to ChatGPT with default settings where you are not saving the chat history
```sh
gpt -u 'This is my first prompt'
```
Send a prompt to ChatGPT with default settings where you are saving the chat history
```sh
gpt -u 'This is my first prompt where I am saving the chat history' -j 'my_chat.json'
```
Send a prompt to ChatGPT where you chnage the model to gpt-4 and set the tempurature to 0 with a system prompt
```sh
gpt -u 'Hello!' -s 'You are a dog' -m 'gpt-4' -t '0'
```
