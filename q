[33mcommit 428ad84e111765b1ba838bfe37508a268d3abea8[m[33m ([m[1;36mHEAD -> [m[1;32mmaster[m[33m, [m[1;31morigin/master[m[33m)[m
Author: Setepenre <pierre.delaunay@outlook.com>
Date:   Sat Mar 10 17:53:48 2018 -0500

    Heavy reformating
    * Split Types and Expression into two different Tree
    * Add the conversion logic between ASTTree and RenderTree
    * Add TIDE entry point

[33mcommit 81d13a9581148eee8598c7ead1f1672f2b7cda9c[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sun Nov 19 16:34:51 2017 -0500

    Tested Function parsing with and without annotations

[33mcommit bbee641f2742f95ae4166fee3c2118dc6b31fbbc[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sat Sep 16 15:08:12 2017 -0400

    temp

[33mcommit 1e976a69b406b226d6c735229006a4c2ce43ad54[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sat Jul 15 04:29:44 2017 -0400

    Replace SFML with SDL -> Event Driven rendering

[33mcommit 10beea2ec06df4d2586a0cb3c8e31a54a46c857d[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sun Jul 2 23:09:23 2017 -0400

    Removed Debug.h macros

[33mcommit 07132c6b2ea3a699b5c0c515281516d562989599[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sat Jul 1 22:14:30 2017 -0400

    Added Syntax Highlighting

[33mcommit c85b3866ff3c586fa2e23e4bb1c3b5395372554e[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Fri Jun 30 17:50:15 2017 -0400

    Fixed Vararg templates on GCC

[33mcommit 0100c312bea9398d31136cccbd18a2ca159951d3[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Fri Jun 30 17:42:14 2017 -0400

    Replaced call to std::cout by true logging primitives

[33mcommit aa983018547ddb2c10bae1d3ce5d72c054ef1f3e[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Fri Jun 30 16:47:27 2017 -0400

    Added Collision box extraction

[33mcommit 25d4ab143fd200bae208748b894836466810f7fd[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Jun 26 22:19:10 2017 -0400

    Added ImGui and worked on the logging util

[33mcommit cfaf57e5dd4b3910dcd58480e53033e219a0956a[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sun Jun 18 21:27:03 2017 -0400

    RenderTree

[33mcommit 17f74abbb92b31471e9630e331d2c0ca84213b1e[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Tue May 23 21:54:03 2017 -0400

    Fix issues with GCC

[33mcommit f6fbb323d8c02a0cd9b6331a1ef830f75d0d0e5d[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Tue May 23 21:25:07 2017 -0400

    Generalization of the AST so we can create multiple AST with different types

[33mcommit 5a1d4e2791004fcce920d6a61914c114a8516a96[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Fri May 5 16:04:47 2017 -0400

    Automatically generate some code

[33mcommit 00bfa48ab9293cd2ac92a3f865f35a5dd853ae7c[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Thu May 4 14:34:32 2017 -0400

    fix wrong include

[33mcommit f4d1ebb09259c71d7e76362bcc7fa147275bf48c[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Thu May 4 14:27:20 2017 -0400

    Started to add types

[33mcommit 03b6393a7066d6c20ff657467f8cc661c5dcc629[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Wed May 3 17:35:04 2017 -0400

    Started adding nodes

[33mcommit 87eec3e5c312af2b5c6ef5ec1d46075efaa9b46f[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Wed May 3 08:51:35 2017 -0400

    Handle docstring

[33mcommit 76f762dbe58ab59a3a47d059e7475edfacb17788[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Wed May 3 00:07:33 2017 -0400

    replaced some lython macros by KIWI

[33mcommit db53d5611e97534a7069a8c77e71350e965b97da[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Tue May 2 23:14:51 2017 -0400

    Added binary_call and unary_call

[33mcommit 3e92153ac0a236a102263907c0c6f4874cd06d11[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Tue May 2 18:35:16 2017 -0400

    implemented the eval of a function call

[33mcommit 841d7f025b0d7e90638875635d88fc12bb863df2[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Tue May 2 17:25:18 2017 -0400

    Modified how functions are implemented

[33mcommit 4115a5600a0336251d4bdedd66a68e43727294d3[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Tue May 2 16:01:58 2017 -0400

    Replaced `Add` by a set of functions

[33mcommit a411155037d761f5756716d2be525f2a99677a5b[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Fri Apr 28 01:22:24 2017 -0400

    Compiles on linux

[33mcommit 56a7d7c939c64df347ccfb954e059cacdf868445[m
Merge: 396dfa7 02e46af
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Wed Apr 26 19:55:17 2017 -0400

    Merging

[33mcommit 396dfa7cc1cf58353b64e5434ad7f395ff461641[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Wed Apr 26 19:47:34 2017 -0400

    Split SFML and Nuklear for better clarity

[33mcommit 02e46af376832120002d80e381876ab4cea2d8f3[m
Author: Pierre <pierre.delaunay@hec.ca>
Date:   Wed Apr 26 12:18:29 2017 -0400

    new nuklear_test

[33mcommit a3de2dc3641cb9fd060f1cc92ddf58afeba97bfa[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Tue Apr 25 11:24:38 2017 -0400

    Refactored nuklear example

[33mcommit 818fd20c68461b357be6d289e9e5e9c2d32e0c4c[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Apr 24 23:02:08 2017 -0400

    Pipeline only make kiwi_test

[33mcommit dfed82154e42f71e472715753c5e72a726076309[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Apr 24 22:55:54 2017 -0400

    Pipeline install sfml

[33mcommit 3f61f36e02e8c830a7de921f0984287f55b3eab5[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Apr 24 22:51:46 2017 -0400

    Pipeline install sfml

[33mcommit d022cff4d800d949f03bed3d8df37fafa38dcf51[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Apr 24 22:47:18 2017 -0400

    nuklear examples working on Windows

[33mcommit dc51741602a90f84e3816f0ebde6aa48ace36e33[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Apr 24 20:05:37 2017 -0400

    Add nuklear

[33mcommit 01205442ebc9ce6b30e42d2a503e811d9c125591[m
Author: Mathieu Duchesneau <duchesneau.mathieu@gmail.com>
Date:   Mon Apr 24 19:57:52 2017 -0400

    adding nuklear

[33mcommit f265845decae908bd721ec357be242166be3ac46[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Apr 24 19:54:10 2017 -0400

    Added nuklear

[33mcommit b5ae54765c884a7824854eaa2714f2bcd9a6cc8c[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Apr 24 19:03:59 2017 -0400

    Added parenthesis handling

[33mcommit 66ce8819b3ec817e169090cda201c08a7a8180aa[m
Author: Mathieu Duchesneau <duchesneau.mathieu@gmail.com>
Date:   Mon Apr 24 16:47:36 2017 -0400

    remove #pragma once

[33mcommit 63bcd47801f4c67360764d2e685e176a6e7e89fd[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Mon Apr 24 16:34:25 2017 -0400

    fix lexer number handling

[33mcommit d55634d309ca321d6ecd2ecc3b6616813b575d7c[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sun Apr 23 10:52:25 2017 -0400

    undef debug macro before defining them

[33mcommit b2db311062b32771bebf56f2dc1be8077a5a3741[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sun Apr 23 07:28:14 2017 -0400

    Added Debug.h

[33mcommit 57a050eaec4fa7e07f0b3588e639125563e1908d[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sat Apr 22 12:54:22 2017 -0400

    Root copy constructor now takes ownership of copied root

[33mcommit e55603b7d77878a67ef3d9a424a5e32f59dc7d51[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sat Apr 22 12:37:10 2017 -0400

    Added a simple parser

[33mcommit 19e7d1aa3a2021b0876c891f70f433c4945b9ff9[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Sat Apr 22 07:13:19 2017 -0400

    Added the ability to borrow a subtree

[33mcommit 220745bd1383e0f826f6fb7ce12fc93721e4f2ee[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Fri Apr 21 22:44:46 2017 -0400

    Memory is freed automatically

[33mcommit 7247613a956353ad13c5ab99e969a29c7ecd691d[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Fri Apr 21 19:07:51 2017 -0400

    Added a free memory visitor

[33mcommit a5309bc9b039f86c8a9bcfbe2baa2798eda75351[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Fri Apr 21 15:03:57 2017 -0400

    typo

[33mcommit 61fc97c956c9262580596df0a9e8f144dbf40485[m
Author: Setepenre <pierre.delaunay@hec.ca>
Date:   Wed Apr 19 18:55:35 2017 -0400

    first commit
