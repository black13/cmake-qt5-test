#include <QTest>
#include <QJSEngine>

class EvaluateWrapper : public QObject
{
    Q_OBJECT
public:
    EvaluateWrapper(QJSEngine *engine)
        : engine(engine)
    {}

public slots:
    QJSValue cppEvaluate(const QString &program)
    {
        return engine->evaluate(program);
    }

private:
    QJSEngine *engine;
};

class Foo : public QObject {
    Q_OBJECT
private slots:
    void t1(); 
};

void Foo::t1()
{   
    QJSEngine engine;
    QJSValue wrapper = engine.newQObject(new EvaluateWrapper(&engine));

    engine.evaluate("testVariable = 42");

    QJSValue function = engine.evaluate("(function(evalWrapper){\n"
                                        "var testVariable = 100; \n"
                                        "try { \n"
                                        "    return evalWrapper.cppEvaluate(\"(function() { return testVariable; })\")\n"
                                        "           ()\n"
                                        "} catch (e) {}\n"
                                        "})");
    QVERIFY(function.isCallable());
    QJSValue result = function.call(QJSValueList() << wrapper);
    QCOMPARE(result.toInt(), 42);
}
QTEST_MAIN(Foo)
#include "main.moc"