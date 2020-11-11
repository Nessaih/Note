### 一、下载代码

1. `git init`: 初始化本地仓库,此步骤可选。
----
2. `git clone http:github.com/[username]/[repository].git`:下载远程仓库至本地。
  + `[username]`:用户账号名字。
  + `[repository]`:远程仓库名字。
----
### 二、上传代码

1. `git init`: 初始化本地仓库
----

2. `git add [files]`:添加本地文件到仓库
  + `[files]`:`.`表示添加当前路径下所有文件，其他情况按照文件(夹)名添加,文件名需要包含扩展名。
----

3. `git commit -m ['string']`:提交暂存区到本地仓库
  + `['string']`: string为需要注释的文字内容，需要使用英文引号包含。
----
4. `git remote add [origin] http:github.com/[username]/[repository].git`:关联远程仓库
  + `[origin]`:远程仓库后，给此厂库命名。此选项可选，不写此选项则`origin`默认为`repository`。
  + `[username]`:用户账号名字。
  + `[repository]`:远程仓库名字。
----
5. `git remote -v`:查看关联远程仓库的详细信息，去掉`-v`则是查看远程厂库所命名字。（此步骤可选）
----
6. `git push -u origin master`:将代码提交到远程仓`repository`的master分支。
----
### 三、备注

1. `http:github.com/[username]/[repository].git`也可写作`git@github.com/[username]/[repository].git`
----
