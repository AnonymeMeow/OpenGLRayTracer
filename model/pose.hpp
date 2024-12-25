#pragma once

class Quaternion
{
public:
    double w, x, y, z;
    Quaternion(double, double, double, double);
    Quaternion operator~() const;
    Quaternion operator-() const;
    Quaternion operator+(const Quaternion&) const;
    Quaternion operator-(const Quaternion&) const;
    Quaternion operator*(const Quaternion&) const;
};

class PoseTransform
{
public:
    Quaternion rotation, translation;
    PoseTransform(Quaternion, Quaternion);
    PoseTransform(double[3], double[3]);
    PoseTransform operator*(const PoseTransform&) const;
    Quaternion operator*(const Quaternion&) const;
};