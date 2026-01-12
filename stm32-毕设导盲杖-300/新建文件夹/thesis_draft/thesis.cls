\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{thesis}[2026/01/12 Thesis Template]

% Use ctexrep for report-style thesis (Chapters)
\LoadClass[a4paper,zihao=-4]{ctexrep}

% Packages
\RequirePackage{geometry}
\geometry{left=3cm,right=2.5cm,top=2.5cm,bottom=2.5cm}

\RequirePackage{graphicx}
%\RequirePackage{wallpaper} % Removed as requested
\RequirePackage{titlesec}
\RequirePackage{fancyhdr}
\RequirePackage{titletoc}
\RequirePackage{amsmath}
\RequirePackage{setspace}
\RequirePackage[colorlinks=true,linkcolor=black,citecolor=black,urlcolor=black]{hyperref}

% Define Custom Fields
\newcommand{\@studentID}{}
\newcommand{\studentID}[1]{\renewcommand{\@studentID}{#1}}

\newcommand{\@theclass}{}
\newcommand{\theclass}[1]{\renewcommand{\@theclass}{#1}}

\newcommand{\@advisor}{}
\newcommand{\advisor}[1]{\renewcommand{\@advisor}{#1}}

\newcommand{\@college}{}
\newcommand{\college}[1]{\renewcommand{\@college}{#1}}

% Cover Page Definition
\newcommand{\makecover}{
    \begin{titlepage}
        \centering
        \vspace*{1.5cm}
        % School Logo
        \includegraphics[width=14cm]{images/school_logo.png} \\
        \vspace{2.5cm}
        
        {\zihao{1}\heiti\bfseries 毕\quad 业\quad 设\quad 计}\\[3.5cm]
        
        {\zihao{3}\heiti\bfseries 题目：\underline{\makebox[11cm][c]{\@title}}}\\[3.5cm]
        
        {\zihao{3}\songti
            \renewcommand{\arraystretch}{1.6}
            \begin{tabular}{rc}
                \textbf{学生姓名：} & \underline{\makebox[9cm][c]{\@author}} \\
                \textbf{学号：} & \underline{\makebox[9cm][c]{\@studentID}} \\
                \textbf{专业班级：} & \underline{\makebox[9cm][c]{\@theclass}} \\
                \textbf{指导教师：} & \underline{\makebox[9cm][c]{\@advisor}} \\
                \textbf{所属学院：} & \underline{\makebox[9cm][c]{\@college}} \\
            \end{tabular}
        }
        
        \vfill
        {\zihao{3}\songti \@date}
        \vspace{2.5cm}
    \end{titlepage}
}

% Abstract Environment
\newenvironment{cnabstract}{
    \chapter*{摘\quad 要}
    \addcontentsline{toc}{chapter}{摘要}
    \songti
}{
    \par
}

\newcommand{\cnkeywords}[1]{
    \vspace{1em}
    \noindent\textbf{关键词：} #1
}

% Section Formatting
\ctexset{
    chapter = {
        format = \zihao{3}\heiti\centering,
        beforeskip = 20pt,
        afterskip = 20pt,
    },
    section = {
        format = \zihao{4}\heiti,
        beforeskip = 15pt,
        afterskip = 15pt,
    },
    subsection = {
        format = \zihao{-4}\heiti,
        beforeskip = 10pt,
        afterskip = 10pt,
    }
}

% Header and Footer
\pagestyle{fancy}
\fancyhf{}
\fancyhead[C]{\zihao{5}\songti \@title}
\fancyfoot[C]{\thepage}
\renewcommand{\headrulewidth}{0.5pt}

\endinput
