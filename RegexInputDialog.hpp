#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

class RegexInputDialog : public QDialog
{
	Q_OBJECT
public:
	explicit RegexInputDialog(QWidget* parent = nullptr);

	void setTitle(const QString& title);
	void setLabelText(const QString& label);
	void setText(const QString& text);
	void setRegExp(const QRegularExpression& regExp);

	QString getLabelText() const;
	QString getText() const;

	static QString getText(QWidget* parent, const QString& title, const QString& label,
		const QString& text, const QRegularExpression& regexp, bool* ok);

private slots:
	void checkValid(QString text);

private:
	QLabel*			  mpLabel = nullptr;
	QLineEdit*		  mpLineEdit = nullptr;
	QDialogButtonBox* mpButtonBox = nullptr;

	QRegularExpression			 mRegexp;
	QRegularExpressionValidator* mpRegexpValidator;
};
