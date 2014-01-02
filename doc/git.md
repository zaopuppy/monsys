
常用git命令
=================================================

## 回滚没有push的commit

http://stackoverflow.com/questions/927358/how-to-undo-the-last-git-commit

This took me a while to figure out, so maybe this will help someone...

There are two ways to "undo" your last commit, depending on whether or not you have already made your commit public (pushed to your remote repository):

### How to undo a local commit

Lets say I committed locally, but now want to remove that commit.

    git log
        commit 101: bad commit    # latest commit, this would be called 'HEAD'
        commit 100: good commit   # second to last commit, this is the one we want

To restore everything back to the way it was prior to the last commit, we need to `reset` to the commit before `HEAD`:

    git reset --soft HEAD^     # use --soft if you want to keep your changes
    git reset --hard HEAD^     # use --hard if you don't care about keeping the changes you made

Now git log will show that our last commit has been removed.

### How to undo a public commit

If you have already made your commits public, you will want to create a new commit which will "revert" the changes you made in your previous commit (current HEAD).

    git revert HEAD

Your changes will now be reverted and ready for you to commit:

    git commit -m 'restoring the file I removed on accident'
    git log
        commit 102: restoring the file I removed on accident
        commit 101: removing a file we dont need
        commit 100: adding a file that we need

For more info, check out [Git Book - Reset, Checkout and Revert](http://book.git-scm.com/4_undoing_in_git_-_reset,_checkout_and_revert.html)



