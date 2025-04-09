# Nozomi 7-Zip 
Interactive version of 7zip to pipe commands easily to the process.

# How to update 
In order to update 7zip and then apply the changes required for the interactive version a possible procedure is outlined below:

- Create a copy of the branches 'master' and 'nozomi-interactive' named 'master-old' and 'nozomi-interactive-old'
- extract in a tmp folder the new 7zip version (https://github.com/ip7z/7zip/tree/main)
- checkout master branch of nozomi 7-zip
- copy here the files of the tmp folder
- commit 
- checkout nozomi-interactive branch of nozomi 7-zip
- copy here the files of the tmp folder
- commit 
- Compare (for example with GitLens) 'master-old' vs 'nozomi-interactive-old'
- apply in nozomi-interactive the changes from the comparison

