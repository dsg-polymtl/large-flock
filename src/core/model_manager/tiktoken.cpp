#include <Python.h>
#include <filesystem>
#include <large_flock/core/model_manager/tiktoken.hpp>

namespace large_flock {

namespace core {

void Tiktoken::SetupPython() {
    // Initialize the Python interpreter
    Py_Initialize();

    // Determine the path to the Python script directory
    char exe_path[4096];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        throw std::runtime_error("Failed to determine the executable path.");
    }
    exe_path[len] = '\0'; // Null-terminate the path
    auto script_dir = std::filesystem::path(exe_path).remove_filename() / "extension/large_flock";

    // Add the Python script directory to sys.path
    auto *sys_path = PySys_GetObject("path");
    if (!sys_path) {
        throw std::runtime_error("Failed to get Python sys.path.");
    }
    auto *folder_path = PyUnicode_FromString(script_dir.string().c_str());
    if (!folder_path) {
        throw std::runtime_error("Failed to create Python string for the folder path.");
    }
    if (PyList_Append(sys_path, folder_path) != 0) {
        throw std::runtime_error("Failed to append folder path to Python sys.path.");
    }
    Py_DECREF(folder_path);
}

int Tiktoken::GetNumTokens(const std::string &str) {
    // Import the Python module
    auto *module_name = PyUnicode_FromString("get_num_tokens");
    if (!module_name) {
        throw std::runtime_error("Failed to create Python string for the module name.");
    }
    auto *module = PyImport_Import(module_name);
    Py_DECREF(module_name);
    if (!module) {
        PyErr_Print();
        throw std::runtime_error("Failed to import Python module 'get_num_tokens'.");
    }

    // Get the Python function from the module
    auto *func = PyObject_GetAttrString(module, "num_tokens");
    Py_DECREF(module);
    if (!func || !PyCallable_Check(func)) {
        PyErr_Print();
        throw std::runtime_error("Cannot find or call the Python function 'num_tokens'.");
    }

    auto *python_input = PyUnicode_FromString(str.c_str());
    if (!python_input) {
        throw std::runtime_error("Failed to create Python string for input.");
    }

    // Call the Python function
    auto *result = PyObject_CallOneArg(func, python_input);
    Py_DECREF(func);
    Py_DECREF(python_input);
    if (!result) {
        PyErr_Print();
        throw std::runtime_error("Failed to call Python function 'num_tokens'.");
    }

    // Extract the integer result from the Python object
    int length = static_cast<int>(PyLong_AsLong(result));
    Py_DECREF(result);

    return length;
}

} // namespace core

} // namespace large_flock