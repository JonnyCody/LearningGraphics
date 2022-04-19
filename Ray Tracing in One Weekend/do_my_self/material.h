//
// Created by jonny on 2022/4/19.
//

#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

/*#include "utility.h"
#include "ray.h"*/
#include "hittable.h"
// struct hit_record;
class material
{
public:
    virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
            ) const = 0;
};

class lambertian : public material
{
public:
    lambertian(const color& c) : albedo(c){}
    virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override
    {
        vec3 scatter = rec.normal + random_unit_vec3();
        if(scatter.near_zero())
        {
            scatter = rec.normal;
        }
        scattered = ray(rec.position, scatter);
        attenuation = albedo;
        return true;
    }
private:
    color albedo;
};

class metal : public material
{
public:
    metal(const color& c, double f) : albedo(c), fuzz(f){}
    virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override
    {
        auto reflected = reflect(unit_vector(r_in.get_direction()), rec.normal);
        scattered = ray(rec.position, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return dot(scattered.get_direction(), rec.normal) > 0;
    }
private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

        vec3 unit_direction = unit_vector(r_in.get_direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract)
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.position, direction);
        return true;
    }

public:
    double ir; // Index of Refraction
};

#endif //RAYTRACING_MATERIAL_H
