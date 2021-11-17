#pragma once

#include <memory>
#include <unordered_map>

#include <object.h>

class Scope {
public:
    explicit Scope(const std::shared_ptr<Scope>& parent = nullptr) : parent_(parent) {
    }

    void DefineVariable(const std::string& name, const std::shared_ptr<Object>& value) {
        namespace_.insert_or_assign(name, value);
    }

    void SetVariable(const std::string& name, const std::shared_ptr<Object>& value) {
        if (namespace_.find(name) != namespace_.end()) {
            namespace_.insert_or_assign(name, value);
        } else if (parent_) {
            parent_->SetVariable(name, value);
        }
    }

    void DelVariable(const std::string& name) {
        namespace_.erase(name);
    }

    bool HasVariable(const std::string& name) const {
        if (namespace_.find(name) != namespace_.end()) {
            return true;
        }
        return parent_ && parent_->HasVariable(name);
    }

    std::shared_ptr<Object> GetVariable(const std::string& name) const {
        auto it = namespace_.find(name);
        if (it != namespace_.end()) {
            return it->second;
        }
        if (parent_) {
            return parent_->GetVariable(name);
        }
        return nullptr;
    }

private:
    std::shared_ptr<Scope> parent_;
    std::unordered_map<std::string, std::shared_ptr<Object>> namespace_;
};
