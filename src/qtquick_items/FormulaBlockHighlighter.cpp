#include "FormulaBlockHighlighter.h"


FormulaBlockHighlighter::FormulaBlockHighlighter(QTextDocument* textDocument)
    : QSyntaxHighlighter(textDocument)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::white);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bv\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    keywordFormat.setForeground(Qt::yellow);
    keywordFormat.setFontWeight(QFont::Bold);
    keywordPatterns.clear();
    keywordPatterns << "\\bx\\b" << "\\by\\b" << "\\bz\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
}

void FormulaBlockHighlighter::highlightBlock(const QString& text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
