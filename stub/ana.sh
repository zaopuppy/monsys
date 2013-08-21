#!/bin/bash

grep "::init" ana.log > init.log
grep "Waiting for response:" ana.log > wait.log
grep "Connected:" ana.log > connect.log

