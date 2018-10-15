#ifndef FORMULABLOCKHIGHLIGHTER_H
#define FORMULABLOCKHIGHLIGHTER_H

#include <QSyntaxHighlighter>


class FormulaBlockHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit FormulaBlockHighlighter(QTextDocument* textDocument = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    //QRegExp commentStartExpression;
    //QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    //QTextCharFormat classFormat;
    //QTextCharFormat singleLineCommentFormat;
    //QTextCharFormat multiLineCommentFormat;
    //QTextCharFormat quotationFormat;
    //QTextCharFormat functionFormat;

};

#endif // FORMULABLOCKHIGHLIGHTER_H
