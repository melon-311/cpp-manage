#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;

#define MAX_STUDENTS 100
#define MAX_COURSES 50
#define MAX_SELECT 200
#define MAX_REBUILD 5
#define MAX_TEACHERS 20

// 重修记录结构体
struct restudy
{
    char semester[10]; // 学期
    double score;      // 重修分数
};

// 选课记录结构体
struct select
{
    char student_id[15];           // 学号
    char course_id[10];            // 课程号
    double usual_score;            // 平时分
    double exam_score;             // 考试分
    double total_score;            // 总分
    restudy rebuilds[MAX_REBUILD]; // 重修记录数组
    int rebuild_cnt;               // 重修次数
};

// 学生结构体
struct Student
{
    char id[15];    // 学号
    char name[20];  // 姓名
    char gender[5]; // 性别
    int age;        // 年龄
    char dorm[15];  // 宿舍
    char phone[15]; // 手机号
    int class_id;   // 班级号
};

// 课程结构体
struct Course
{
    char course_id[10]; // 课程编号
    char name[30];      // 课程名
    char time[30];      // 上课时间
    double credit;      // 学分
};

// 教师结构体
struct Teacher
{
    char id[15];   // 教师工号
    char name[20]; // 教师姓名
    char pwd[20];  // 登录密码
};

// 全局数据存储
Student students[MAX_STUDENTS];
int student_cnt = 0;
Course courses[MAX_COURSES];
int course_cnt = 0;
select selects[MAX_SELECT];
int select_cnt = 0;
Teacher teachers[MAX_TEACHERS];
int teacher_cnt = 0;

char current_user[20]; // 当前登录账号
int current_role;      // 当前角色 0管理员 1学生 2教师

// 函数声明
int find_student(const char *id);
int find_course(const char *cid);
int find_select(const char *sid, const char *cid);
int find_teacher(const char *id);
double calc_total_credit(const char *sid);

// 计算绩点
double get_gpa(double score)
{
    if (score >= 90)
        return 4.0;
    if (score >= 80)
        return 3.0;
    if (score >= 70)
        return 2.0;
    if (score >= 60)
        return 1.0;
    return 0.0;
}

// 计算学生总有效学分（及格才算）
double calc_total_credit(const char *sid)
{
    double total = 0;
    for (int i = 0; i < select_cnt; i++)
    {
        if (strcmp(selects[i].student_id, sid) == 0 && selects[i].total_score >= 60)
        {
            int cidx = find_course(selects[i].course_id);
            if (cidx != -1)
            {
                total += courses[cidx].credit;
            }
        }
    }
    return total;
}

// 从文件加载所有数据
void load_data()
{
    FILE *f;

    f = fopen("students.txt", "r");
    if (f)
    {
        fscanf(f, "%d", &student_cnt);
        for (int i = 0; i < student_cnt; i++)
        {
            fscanf(f, "%s %s %s %d %s %s %d",
                   students[i].id, students[i].name, students[i].gender, &students[i].age,
                   students[i].dorm, students[i].phone, &students[i].class_id);
        }
        fclose(f);
    }

    f = fopen("courses.txt", "r");
    if (f)
    {
        fscanf(f, "%d", &course_cnt);
        for (int i = 0; i < course_cnt; i++)
        {
            fscanf(f, "%s %s %s %lf",
                   courses[i].course_id, courses[i].name, courses[i].time, &courses[i].credit);
        }
        fclose(f);
    }

    f = fopen("selects.txt", "r");
    if (f)
    {
        fscanf(f, "%d", &select_cnt);
        for (int i = 0; i < select_cnt; i++)
        {
            fscanf(f, "%s %s %lf %lf %lf %d",
                   selects[i].student_id, selects[i].course_id,
                   &selects[i].usual_score, &selects[i].exam_score, &selects[i].total_score,
                   &selects[i].rebuild_cnt);
            for (int r = 0; r < selects[i].rebuild_cnt; r++)
            {
                fscanf(f, "%s %lf", selects[i].rebuilds[r].semester, &selects[i].rebuilds[r].score);
            }
        }
        fclose(f);
    }

    f = fopen("teachers.txt", "r");
    if (f)
    {
        fscanf(f, "%d", &teacher_cnt);
        for (int i = 0; i < teacher_cnt; i++)
        {
            fscanf(f, "%s %s %s", teachers[i].id, teachers[i].name, teachers[i].pwd);
        }
        fclose(f);
    }
}

// 保存所有数据到文件
void save_data()
{
    FILE *f;

    f = fopen("students.txt", "w");
    fprintf(f, "%d\n", student_cnt);
    for (int i = 0; i < student_cnt; i++)
    {
        fprintf(f, "%s %s %s %d %s %s %d\n",
                students[i].id, students[i].name, students[i].gender, students[i].age,
                students[i].dorm, students[i].phone, students[i].class_id);
    }
    fclose(f);

    f = fopen("courses.txt", "w");
    fprintf(f, "%d\n", course_cnt);
    for (int i = 0; i < course_cnt; i++)
    {
        fprintf(f, "%s %s %s %.1f\n",
                courses[i].course_id, courses[i].name, courses[i].time, courses[i].credit);
    }
    fclose(f);

    f = fopen("selects.txt", "w");
    fprintf(f, "%d\n", select_cnt);
    for (int i = 0; i < select_cnt; i++)
    {
        fprintf(f, "%s %s %.1f %.1f %.1f %d\n",
                selects[i].student_id, selects[i].course_id,
                selects[i].usual_score, selects[i].exam_score, selects[i].total_score,
                selects[i].rebuild_cnt);
        for (int r = 0; r < selects[i].rebuild_cnt; r++)
        {
            fprintf(f, "%s %.1f\n", selects[i].rebuilds[r].semester, selects[i].rebuilds[r].score);
        }
    }
    fclose(f);

    f = fopen("teachers.txt", "w");
    fprintf(f, "%d\n", teacher_cnt);
    for (int i = 0; i < teacher_cnt; i++)
    {
        fprintf(f, "%s %s %s\n", teachers[i].id, teachers[i].name, teachers[i].pwd);
    }
    fclose(f);
}

// 查找学生，返回下标，无则-1
int find_student(const char *id)
{
    for (int i = 0; i < student_cnt; i++)
        if (strcmp(students[i].id, id) == 0)
            return i;
    return -1;
}
// 查找课程
int find_course(const char *cid)
{
    for (int i = 0; i < course_cnt; i++)
        if (strcmp(courses[i].course_id, cid) == 0)
            return i;
    return -1;
}
// 查找选课记录（学号+课程号唯一）
int find_select(const char *sid, const char *cid)
{
    for (int i = 0; i < select_cnt; i++)
        if (strcmp(selects[i].student_id, sid) == 0 && strcmp(selects[i].course_id, cid) == 0)
            return i;
    return -1;
}
// 查找教师
int find_teacher(const char *id)
{
    for (int i = 0; i < teacher_cnt; i++)
        if (strcmp(teachers[i].id, id) == 0)
            return i;
    return -1;
}

// ==========管理员功能函数==========
// 查看全部学生
void admin_show_all_students()
{
    system("cls");
    cout << "============== 学生信息列表 ==============\n";
    if (student_cnt == 0)
    {
        cout << "    暂无学生信息\n";
        system("pause");
        return;
    }
    for (int i = 0; i < student_cnt; i++)
    {
        cout << "学号：" << students[i].id
             << "        姓名：" << students[i].name
             << "   性别：" << students[i].gender
             << "     年龄：" << students[i].age
             << "     宿舍：" << students[i].dorm
             << "   电话：" << students[i].phone
             << "   班级：" << students[i].class_id << "\n";
    }
    system("pause");
}

// 查看全部课程
void admin_show_all_courses()
{
    system("cls");
    cout << "============== 课程信息列表 ==============\n";
    if (course_cnt == 0)
    {
        cout << "   暂无课程信息\n";
        system("pause");
        return;
    }
    for (int i = 0; i < course_cnt; i++)
    {
        cout << "课程编号：" << courses[i].course_id
             << "   课程名称：" << courses[i].name
             << "  上课时间：" << courses[i].time
             << "  课程学分：" << courses[i].credit << "\n";
    }
    system("pause");
}

// 查看全部教师
void admin_show_all_teachers()
{
    system("cls");
    cout << "============== 教师信息列表 ==============\n";
    if (teacher_cnt == 0)
    {
        cout << "   暂无教师信息\n";
        system("pause");
        return;
    }
    for (int i = 0; i < teacher_cnt; i++)
    {
        cout << "工号：" << teachers[i].id
             << "        姓名：" << teachers[i].name
             << "     密码：" << teachers[i].pwd << "\n";
    }
    system("pause");
}

// 添加学生
void admin_add_student()
{
    system("cls");
    cout << "============== 添加学生 ==============\n";
    if (student_cnt >= MAX_STUDENTS)
    {
        cout << "学生数量已达上限，无法新增\n";
        system("pause");
        return;
    }
    Student s;
    cout << "输入学号：";
    cin >> s.id;
    if (find_student(s.id) != -1)
    {
        cout << "该学号已存在\n";
        system("pause");
        return;
    }
    cout << "输入姓名：";
    cin >> s.name;
    cout << "输入性别：";
    cin >> s.gender;
    cout << "输入年龄：";
    cin >> s.age;
    cout << "输入宿舍：";
    cin >> s.dorm;
    cout << "输入电话：";
    cin >> s.phone;
    cout << "输入班级：";
    cin >> s.class_id;
    students[student_cnt++] = s;
    save_data();
    cout << "\n学生添加成功\n";
    system("pause");
}

// 修改学生信息
void admin_modify_student()
{
    system("cls");
    cout << "============== 修改学生信息 ==============\n";
    char id[15];
    cout << "请输入要修改的学生学号：";
    cin >> id;
    int i = find_student(id);
    if (i == -1)
    {
        cout << "未找到该学生\n";
        system("pause");
        return;
    }
    cout << "\n请输入新信息：\n";
    cout << "姓名：";
    cin >> students[i].name;
    cout << "性别：";
    cin >> students[i].gender;
    cout << "年龄：";
    cin >> students[i].age;
    cout << "宿舍：";
    cin >> students[i].dorm;
    cout << "电话：";
    cin >> students[i].phone;
    cout << "班级：";
    cin >> students[i].class_id;
    save_data();
    cout << "\n信息修改成功\n";
    system("pause");
}

// 删除学生
void admin_delete_student()
{
    system("cls");
    cout << "============== 删除学生 ==============\n";
    char id[15];
    cout << "请输入要删除的学生学号：";
    cin >> id;
    int pos = find_student(id);
    if (pos == -1)
    {
        cout << "未找到该学生\n";
        system("pause");
        return;
    }
    for (int i = pos; i < student_cnt - 1; i++)
        students[i] = students[i + 1];
    student_cnt--;
    save_data();
    cout << "\n删除成功\n";
    system("pause");
}

// 添加课程
void admin_add_course()
{
    system("cls");
    cout << "============== 添加课程 ==============\n";
    if (course_cnt >= MAX_COURSES)
    {
        cout << "课程数量已达上限，无法新增\n";
        system("pause");
        return;
    }
    Course c;
    cout << "输入课程编号：";
    cin >> c.course_id;
    if (find_course(c.course_id) != -1)
    {
        cout << "该课程编号已存在\n";
        system("pause");
        return;
    }
    cout << "输入课程名称：";
    cin >> c.name;
    cout << "输入上课时间：";
    cin >> c.time;
    cout << "输入学分：";
    cin >> c.credit;
    courses[course_cnt++] = c;
    save_data();
    cout << "\n课程添加成功\n";
    system("pause");
}

// 修改课程
void admin_modify_course()
{
    system("cls");
    cout << "============== 修改课程 ==============\n";
    char id[10];
    cout << "请输入要修改的课程编号：";
    cin >> id;
    int i = find_course(id);
    if (i == -1)
    {
        cout << "未找到该课程\n";
        system("pause");
        return;
    }
    cout << "\n请输入新信息：\n";
    cout << "课程名称：";
    cin >> courses[i].name;
    cout << "上课时间：";
    cin >> courses[i].time;
    cout << "学分：";
    cin >> courses[i].credit;
    save_data();
    cout << "\n修改成功\n";
    system("pause");
}

// 删除课程
void admin_delete_course()
{
    system("cls");
    cout << "============== 删除课程 ==============\n";
    char id[10];
    cout << "请输入要删除的课程编号：";
    cin >> id;
    int pos = find_course(id);
    if (pos == -1)
    {
        cout << "未找到该课程\n";
        system("pause");
        return;
    }
    for (int i = pos; i < course_cnt - 1; i++)
        courses[i] = courses[i + 1];
    course_cnt--;
    save_data();
    cout << "\n删除成功\n";
    system("pause");
}

// 添加教师
void admin_add_teacher()
{
    system("cls");
    cout << "============== 添加教师 ==============\n";
    if (teacher_cnt >= MAX_TEACHERS)
    {
        cout << "教师数量已达上限，无法新增\n";
        system("pause");
        return;
    }
    Teacher t;
    cout << "输入教师工号：";
    cin >> t.id;
    if (find_teacher(t.id) != -1)
    {
        cout << "该工号已存在\n";
        system("pause");
        return;
    }
    cout << "输入姓名：";
    cin >> t.name;
    cout << "输入登录密码：";
    cin >> t.pwd;
    teachers[teacher_cnt++] = t;
    save_data();
    cout << "\n教师添加成功\n";
    system("pause");
}

// 修改教师信息
void admin_modify_teacher()
{
    system("cls");
    cout << "============== 修改教师信息 ==============\n";
    char id[15];
    cout << "请输入要修改的教师工号：";
    cin >> id;
    int i = find_teacher(id);
    if (i == -1)
    {
        cout << "未找到该教师\n";
        system("pause");
        return;
    }
    cout << "\n请输入新信息：\n";
    cout << "姓名：";
    cin >> teachers[i].name;
    cout << "登录密码：";
    cin >> teachers[i].pwd;
    save_data();
    cout << "\n修改成功\n";
    system("pause");
}

// 删除教师
void admin_delete_teacher()
{
    system("cls");
    cout << "============== 删除教师 ==============\n";
    char id[15];
    cout << "请输入要删除的教师工号：";
    cin >> id;
    int pos = find_teacher(id);
    if (pos == -1)
    {
        cout << "未找到该教师\n";
        system("pause");
        return;
    }
    for (int i = pos; i < teacher_cnt - 1; i++)
        teachers[i] = teachers[i + 1];
    teacher_cnt--;
    save_data();
    cout << "\n删除成功\n";
    system("pause");
}

// 管理员主菜单
void admin_menu()
{
    int op;
    while (1)
    {
        system("cls");
        cout << "=========================================\n";
        cout << "                  管理员操作菜单                  \n";
        cout << "=========================================\n";
        cout << "  1. 添加学生        2. 修改学生        3. 删除学生    \n";
        cout << "  4. 添加课程       5. 修改课程       6. 删除课程   \n";
        cout << "  7. 添加教师      8. 修改教师      9. 删除教师  \n";
        cout << " 10. 查看全部学生   11. 查看全部课程  12. 查看全部教师\n";
        cout << "  0. 返回上级菜单                               \n";
        cout << "=========================================\n";
        cout << " 请输入选项：";
        cin >> op;
        if (op == 0)
            break;
        switch (op)
        {
        case 1:
            admin_add_student();
            break;
        case 2:
            admin_modify_student();
            break;
        case 3:
            admin_delete_student();
            break;
        case 4:
            admin_add_course();
            break;
        case 5:
            admin_modify_course();
            break;
        case 6:
            admin_delete_course();
            break;
        case 7:
            admin_add_teacher();
            break;
        case 8:
            admin_modify_teacher();
            break;
        case 9:
            admin_delete_teacher();
            break;
        case 10:
            admin_show_all_students();
            break;
        case 11:
            admin_show_all_courses();
            break;
        case 12:
            admin_show_all_teachers();
            break;
        default:
            cout << "输入选项无效\n";
            system("pause");
        }
    }
}

// ==========学生功能函数==========
// 选课
void student_select()
{
    system("cls");
    cout << "============== 选课功能 ==============\n";
    cout << " 可选课程列表：\n\n";
    for (int i = 0; i < course_cnt; i++)
        cout << courses[i].course_id << "  " << courses[i].name << "  学分：" << courses[i].credit << "\n";
    char cid[10];
    cout << "\n输入要选择的课程编号：";
    cin >> cid;
    if (find_course(cid) == -1)
    {
        cout << "课程不存在\n";
        system("pause");
        return;
    }
    if (find_select(current_user, cid) != -1)
    {
        cout << "你已经选过该课程，不可重复选课\n";
        system("pause");
        return;
    }
    select e;
    strcpy(e.student_id, current_user);
    strcpy(e.course_id, cid);
    e.usual_score = e.exam_score = e.total_score = -1;
    e.rebuild_cnt = 0;
    selects[select_cnt++] = e;
    save_data();
    cout << "\n选课成功\n";
    system("pause");
}

// 查看课表
void student_schedule()
{
    system("cls");
    cout << "============== 我的课表 ==============\n";
    bool f = 0;
    for (int i = 0; i < select_cnt; i++)
    {
        if (!strcmp(selects[i].student_id, current_user))
        {
            int c = find_course(selects[i].course_id);
            if (c != -1)
            {
                cout << courses[c].name << "  上课时间：" << courses[c].time << "\n";
                f = 1;
            }
        }
    }
    if (!f)
        cout << "你还未选择任何课程\n";
    system("pause");
}

// 查询成绩
void student_query_score()
{
    system("cls");
    cout << "============== 成绩查询 ==============\n";
    bool f = 0;
    for (int i = 0; i < select_cnt; i++)
    {
        if (!strcmp(selects[i].student_id, current_user) && selects[i].total_score >= 0)
        {
            int c = find_course(selects[i].course_id);
            if (c != -1)
            {
                f = 1;
                cout << "\n课程：" << courses[c].name
                     << "   总分：" << selects[i].total_score
                     << "     绩点：" << get_gpa(selects[i].total_score)
                     << "  获得学分：" << (selects[i].total_score >= 60 ? courses[c].credit : 0.0) << "\n";
                if (selects[i].rebuild_cnt > 0)
                {
                    cout << "   重修次数：" << selects[i].rebuild_cnt << "\n";
                    for (int r = 0; r < selects[i].rebuild_cnt; r++)
                        cout << "      第" << r + 1 << "次重修 学期：" << selects[i].rebuilds[r].semester << "   重修分数：" << selects[i].rebuilds[r].score << "\n";
                }
            }
        }
    }
    cout << "\n==========================\n";
    cout << " 累计有效总学分：" << calc_total_credit(current_user) << "\n";
    if (!f)
        cout << "暂无已录入的课程成绩\n";
    system("pause");
}

// 统计学分与挂科数量
void student_credit_and_fail()
{
    system("cls");
    cout << "============== 学分与挂科统计 ==============\n";
    double c = 0;
    int f = 0;
    for (int i = 0; i < select_cnt; i++)
    {
        if (!strcmp(selects[i].student_id, current_user) && selects[i].total_score >= 0)
        {
            int ci = find_course(selects[i].course_id);
            if (ci != -1)
            {
                if (selects[i].total_score >= 60)
                    c += courses[ci].credit;
                else
                    f++;
            }
        }
    }
    cout << " 已获得有效学分：" << c << "\n";
    cout << " 不及格课程总数：" << f << "\n";
    system("pause");
}

// 重修分析
void student_rebuild_analysis()
{
    system("cls");
    cout << "============== 重修记录分析 ==============\n";
    bool f = 0;
    for (int i = 0; i < select_cnt; i++)
    {
        if (!strcmp(selects[i].student_id, current_user) && selects[i].rebuild_cnt > 0)
        {
            int c = find_course(selects[i].course_id);
            if (c == -1)
                continue;
            f = 1;
            cout << courses[c].name << "    历次重修分数：";
            for (int r = 0; r < selects[i].rebuild_cnt; r++)
                cout << selects[i].rebuilds[r].score << "    ";
            cout << "最终成绩：" << selects[i].total_score << "\n";
        }
    }
    if (!f)
        cout << "你没有任何重修记录\n";
    system("pause");
}

// 学生菜单
void student_menu()
{
    int op;
    while (1)
    {
        system("cls");
        cout << "=========================================\n";
        cout << "              学生功能菜单                    \n";
        cout << "=========================================\n";
        cout << "  1. 选课          2. 查看课表       3. 查询成绩   \n";
        cout << "  4. 学分挂科统计      5. 重修记录分析         0. 返回上级菜单   \n";
        cout << "=========================================\n";
        cout << " 请输入选项：";
        cin >> op;
        if (op == 0)
            break;
        switch (op)
        {
        case 1:
            student_select();
            break;
        case 2:
            student_schedule();
            break;
        case 3:
            student_query_score();
            break;
        case 4:
            student_credit_and_fail();
            break;
        case 5:
            student_rebuild_analysis();
            break;
        default:
            cout << "输入选项无效\n";
            system("pause");
        }
    }
}

// ==========教师功能函数==========
// 录入成绩
void teacher_input_score()
{
    system("cls");
    cout << "============== 成绩录入 ==============\n";
    char sid[15], cid[10], sem[10];
    double u, e;
    cout << "输入学生学号：";
    cin >> sid;
    cout << "输入课程编号：";
    cin >> cid;
    int i = find_select(sid, cid);
    if (i == -1)
    {
        cout << "该学生未选择此课程\n";
        system("pause");
        return;
    }

    // 输入合法平时分
    while (true)
    {
        cout << "输入平时成绩：";
        cin >> u;
        if (u >= 0 && u <= 100)
            break;
        cout << "成绩范围必须在0~100之间，请重新输入\n";
    }
    // 输入合法考试分
    while (true)
    {
        cout << "输入期末成绩：";
        cin >> e;
        if (e >= 0 && e <= 100)
            break;
        cout << "成绩范围必须在0~100之间，请重新输入\n";
    }

    double t = u * 0.3 + e * 0.7;
    char r = 'n';
    // 如果已有旧成绩，询问是否保存为重修记录
    if (selects[i].total_score >= 0)
    {
        cout << "\n该学生已有旧成绩，是否将旧成绩保存为重修记录(y/n)：";
        cin >> r;
    }
    if (r == 'y' || r == 'Y')
    {
        if (selects[i].rebuild_cnt >= MAX_REBUILD)
        {
            cout << "重修记录数量已达上限\n";
            system("pause");
            return;
        }
        cout << "输入本次重修对应的学期：";
        cin >> sem;
        strcpy(selects[i].rebuilds[selects[i].rebuild_cnt].semester, sem);
        selects[i].rebuilds[selects[i].rebuild_cnt++].score = selects[i].total_score;
    }
    selects[i].usual_score = u;
    selects[i].exam_score = e;
    selects[i].total_score = t;
    save_data();
    cout << "\n成绩录入完成，总分：" << t << "     对应绩点：" << get_gpa(t) << "\n";
    system("pause");
}

// 教师菜单
void teacher_menu()
{
    int op;
    while (1)
    {
        system("cls");
        cout << "=========================================\n";
        cout << "                教师功能菜单                    \n";
        cout << "=========================================\n";
        cout << "  1. 录入成绩                            \n";
        cout << "  0. 返回上级菜单                          \n";
        cout << "=========================================\n";
        cout << " 请输入选项：";
        cin >> op;
        if (op == 0)
            break;
        if (op == 1)
            teacher_input_score();
        else
        {
            cout << "输入选项无效\n";
            system("pause");
        }
    }
}

// ==========全局统计功能==========
// 按班级统计学生成绩学分
void stat_by_class()
{
    system("cls");
    cout << "============== 按班级统计成绩 ==============\n";
    int c;
    cout << "输入需要统计的班级号：";
    cin >> c;
    cout << "\n班级 " << c << " 学生成绩详情：\n\n";
    for (int i = 0; i < student_cnt; i++)
    {
        if (students[i].class_id == c)
        {
            cout << "学生姓名：" << students[i].name << "    有效总学分：" << calc_total_credit(students[i].id) << "\n";
            for (int j = 0; j < select_cnt; j++)
            {
                if (!strcmp(selects[j].student_id, students[i].id) && selects[j].total_score >= 0)
                {
                    int ci = find_course(selects[j].course_id);
                    if (ci != -1)
                        cout << "  " << courses[ci].name << "  总分：" << selects[j].total_score << "\n";
                }
            }
            cout << "--------------------------\n";
        }
    }
    system("pause");
}

// 按课程统计选课学生成绩
void stat_by_course()
{
    system("cls");
    cout << "============== 按课程统计成绩 ==============\n";
    char c[10];
    cout << "输入课程编号：";
    cin >> c;
    int i = find_course(c);
    if (i == -1)
    {
        cout << "课程不存在\n";
        system("pause");
        return;
    }
    cout << "\n课程名称：" << courses[i].name << "  学分：" << courses[i].credit << "\n\n";
    for (int j = 0; j < select_cnt; j++)
    {
        if (!strcmp(selects[j].course_id, c) && selects[j].total_score >= 0)
        {
            int s = find_student(selects[j].student_id);
            if (s != -1)
                cout << students[s].name << "  总分：" << selects[j].total_score << "\n";
        }
    }
    system("pause");
}

// 全体学生学分挂科总统计
void stat_all_credit_fail()
{
    system("cls");
    cout << "============== 全体学生学分挂科总表 ==============\n";
    for (int i = 0; i < student_cnt; i++)
    {
        double cc = 0;
        int f = 0;
        for (int j = 0; j < select_cnt; j++)
        {
            if (!strcmp(selects[j].student_id, students[i].id) && selects[j].total_score >= 0)
            {
                int ci = find_course(selects[j].course_id);
                if (ci != -1)
                {
                    if (selects[j].total_score >= 60)
                        cc += courses[ci].credit;
                    else
                        f++;
                }
            }
        }
        cout << students[i].name << "  有效学分：" << cc << "        不及格科目数：" << f << "\n";
    }
    system("pause");
}

// 登录函数
int login()
{
    system("cls");
    cout << "=========================================\n";
    cout << "               用户登录                   \n";
    cout << "=========================================\n";
    char n[20], p[20];
    cout << "账号：";
    cin >> n;
    cout << "密码：";
    cin >> p;

    // 管理员账号固定admin/123
    if (!strcmp(n, "admin") && !strcmp(p, "123"))
    {
        strcpy(current_user, n);
        return 0;
    }

    // 教师登录
    int t = find_teacher(n);
    if (t != -1 && !strcmp(teachers[t].pwd, p))
    {
        strcpy(current_user, n);
        return 2;
    }

    // 学生默认密码123456
    if (find_student(n) != -1 && !strcmp(p, "123456"))
    {
        strcpy(current_user, n);
        return 1;
    }

    cout << "\n账号或密码错误！\n";
    system("pause");
    return -1;
}

int main()
{
    load_data();
    while (1)
    {
        system("cls");
        cout << "=========================================\n";
        cout << "           学生成绩管理系统              \n";
        cout << "=========================================\n";
        cout << "              1. 用户登录                  \n";
        cout << "              0. 退出系统                         \n";
        cout << "=========================================\n";
        int ch;
        cout << "请输入操作：";
        cin >> ch;
        if (ch == 0)
        {
            system("cls");
            save_data();
            cout << "数据已保存，感谢使用，再见！\n";
            system("pause");
            break;
        }
        if (ch != 1)
            continue;

        int r = login();
        if (r == -1)
            continue;
        current_role = r;

        int op;
        while (1)
        {
            system("cls");
            cout << "=========================================\n";
            cout << "                   功能总菜单                    \n";
            cout << "=========================================\n";
            cout << "  1. 角色专属操作       2. 按班级统计            \n";
            cout << "  3. 按课程统计      4. 全体学生学分统计          \n";
            cout << "  0. 退出登录                              \n";
            cout << "=========================================\n";
            cout << " 请输入选项：";
            cin >> op;
            if (op == 0)
                break;
            if (op == 1)
            {
                if (current_role == 0)
                    admin_menu();
                else if (current_role == 1)
                    student_menu();
                else
                    teacher_menu();
            }
            else if (op == 2)
                stat_by_class();
            else if (op == 3)
                stat_by_course();
            else if (op == 4)
                stat_all_credit_fail();
            else
            {
                cout << "输入选项无效\n";
                system("pause");
            }
        }
    }
    return 0;
}