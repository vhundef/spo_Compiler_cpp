//
// Created by vhundef on 28.04.2020.
//
#include <Token.hpp>
#include "gtest/gtest.h"

TEST(token, unregisteredTokenTypeDetection) {
    Token token("=");
    EXPECT_EQ(token.getType(), Token::tokenType::Undefined);
}

TEST(token, assignmentTokenTypeDetection) {
    Token token(":=");
    EXPECT_EQ(token.getType(), Token::tokenType::Assignment);
}

TEST(token, equalityTokenTypeDetection) {
    Token token("==");
    EXPECT_EQ(token.getType(), Token::tokenType::Comparison);
}

TEST(token, keywordTokenTypeDetection) {
    Token token("if");
    EXPECT_EQ(token.getType(), Token::tokenType::Keyword);
}

TEST(token, mathOpTokenTypeDetection) {
    Token token("+");
    EXPECT_EQ(token.getType(), Token::tokenType::MathPlus);
}


TEST(token, equalityTokenTypeTranslation) {
    Token token("==");
    EXPECT_EQ(Token::typeToString(token.getType()), "Comparison");
}

TEST(token, keywordTokenTypeTranslation) {
    Token token("if");
    EXPECT_EQ(Token::typeToString(token.getType()), "Keyword");
}

TEST(token, mathOpTokenTypeTranslation) {
    Token token("+");
    EXPECT_EQ(Token::typeToString(token.getType()), "MathPlus");
}

TEST(token, idOpTokenTypeTranslation) {
    Token token("i");
    EXPECT_EQ(Token::typeToString(token.getType()), "Id");
}
TEST(token, numberOpTokenTypeTranslation) {
    Token token("2");
    EXPECT_EQ(Token::typeToString(token.getType()), "Num");
}
TEST(token, realOpTokenTypeTranslation) {
    Token token("5.5");
    EXPECT_EQ(Token::typeToString(token.getType()), "Num");
}