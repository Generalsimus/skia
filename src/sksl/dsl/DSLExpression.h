/*
 * Copyright 2020 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_DSL_EXPRESSION
#define SKSL_DSL_EXPRESSION

#include "include/private/base/SkAssert.h"
#include "src/sksl/SkSLPosition.h"
#include "src/sksl/ir/SkSLExpression.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#if defined(__has_cpp_attribute) && __has_cpp_attribute(clang::reinitializes)
#define SK_CLANG_REINITIALIZES [[clang::reinitializes]]
#else
#define SK_CLANG_REINITIALIZES
#endif

namespace SkSL {
namespace dsl {

class DSLType;
struct DSLVarBase;

/**
 * Represents an expression such as 'cos(x)' or 'a + b'.
 */
class DSLExpression {
public:
    DSLExpression(const DSLExpression&) = delete;

    DSLExpression(DSLExpression&&);

    DSLExpression();

    /**
     * Creates an expression representing a variable reference.
     */
    DSLExpression(DSLVarBase& var, Position pos = {});

    DSLExpression(DSLVarBase&& var, Position pos = {});

    // If expression is null, returns Poison
    explicit DSLExpression(std::unique_ptr<SkSL::Expression> expression, Position pos = {});

    static DSLExpression Poison(Position pos = {});

    ~DSLExpression();

    DSLType type() const;

    std::string description() const;

    Position position() const;

    void setPosition(Position pos);

    /**
     * Performs assignment, like the '=' operator.
     */
    DSLExpression assign(DSLExpression other);

    /**
     * Returns true if this object contains an expression. DSLExpressions which were created with
     * the empty constructor or which have already been release()ed do not have a value.
     * DSLExpressions created with errors are still considered to have a value (but contain poison).
     */
    bool hasValue() const {
        return fExpression != nullptr;
    }

    /**
     * Returns true if this object contains an expression which is not poison.
     */
    bool isValid() const;

    SK_CLANG_REINITIALIZES void swap(DSLExpression& other);

    /**
     * Invalidates this object and returns the SkSL expression it represents. It is an error to call
     * this on an invalid DSLExpression.
     */
    std::unique_ptr<SkSL::Expression> release() {
        SkASSERT(this->hasValue());
        return std::move(fExpression);
    }

    /**
     * Calls release if this expression has a value, otherwise returns null.
     */
    std::unique_ptr<SkSL::Expression> releaseIfPossible() {
        return std::move(fExpression);
    }

private:
    std::unique_ptr<SkSL::Expression> fExpression;

    friend class DSLCore;
    friend class DSLWriter;
};

} // namespace dsl

} // namespace SkSL

template <typename T> struct sk_is_trivially_relocatable;

template <>
struct sk_is_trivially_relocatable<SkSL::dsl::DSLExpression> : std::true_type {};

#endif
