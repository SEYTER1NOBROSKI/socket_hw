#!/bin/bash

gnome-terminal -- sh -c 'echo "Starting a new program in a separate terminal"; ./server_proxy; exec bash'

gnome-terminal -- sh -c 'echo "Starting a new program in a separate terminal"; ./client_proxy; exec bash'

gnome-terminal -- sh -c 'echo "Starting a new program in a separate terminal"; ./server; exec bash'

gnome-terminal -- sh -c 'echo "Starting a new program in a separate terminal"; ./client; exec bash'