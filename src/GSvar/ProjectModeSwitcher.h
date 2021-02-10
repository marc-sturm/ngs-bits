#ifndef PROJECTMODESWITCHER_H
#define PROJECTMODESWITCHER_H

#include <QSharedPointer>
#include "Exceptions.h"

class ProjectModeSwitcher
{
public:
	static ProjectModeSwitcher& instance();
	static void setIsLocalProject(QSharedPointer<bool> is_local_project);
	static bool isLocalProject();
	static void setProjectFileName(QSharedPointer<QString> project_file_name);
	static const QSharedPointer<QString> projectFileName();

protected:
	ProjectModeSwitcher();
	~ProjectModeSwitcher();

private:
	QSharedPointer<bool> is_local_project_;
	QSharedPointer<QString> project_file_name_;
};

#endif // PROJECTMODESWITCHER_H
