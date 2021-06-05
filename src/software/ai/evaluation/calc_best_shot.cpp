#include "software/ai/evaluation/calc_best_shot.h"

std::optional<Shot> calcBestShotOnGoal(const Segment &goal_post, const Point &shot_origin,
                                       const std::vector<Robot> &robot_obstacles,
                                       TeamType goal, double radius)
{
    size_t max_num_obstacles = robot_obstacles.empty() ? 1 : robot_obstacles.size();

    Angle pos_post_angle = (goal_post.getStart() - shot_origin).orientation();
    Angle neg_post_angle = (goal_post.getEnd() - shot_origin).orientation();

    std::vector<ObstacleAngleSegment> obstacles;
    obstacles.reserve(max_num_obstacles);

    if (goal == TeamType::ENEMY)
    {

        EnemyAngleMap angle_map =
                EnemyAngleMap(pos_post_angle, neg_post_angle, max_num_obstacles);


        for (const Robot &robot_obstacle : robot_obstacles)
        {
            Point enemy_robot_pos    = robot_obstacle.position();
            Vector perpendicular_vec = (enemy_robot_pos - shot_origin).perpendicular();

            Vector one_end_vec = perpendicular_vec.normalize(radius);

            Point one_end   = enemy_robot_pos + one_end_vec;
            Point other_end = enemy_robot_pos - one_end_vec;

            Vector top_vec    = one_end - shot_origin;
            Vector bottom_vec = other_end - shot_origin;

            Angle top_angle    = top_vec.orientation();
            Angle bottom_angle = bottom_vec.orientation();

            if (bottom_angle > angle_map.getAngleSegment().getAngleTop() ||
                top_angle < angle_map.getAngleSegment().getAngleBottom())
            {
                continue;
            }

            ObstacleAngleSegment non_viable_angle_seg = ObstacleAngleSegment(top_angle, bottom_angle);
            obstacles.emplace_back(non_viable_angle_seg);
        }

        std::sort(obstacles.begin(), obstacles.end(), [](ObstacleAngleSegment &a, ObstacleAngleSegment &b) -> int {
            if (a < b) {
                return false;
            } else if (a > b) {
                return true;
            }

            return false;
        });

        for (ObstacleAngleSegment &obstacle_angle_seg : obstacles) {
            angle_map.addNonViableAngleSegment(obstacle_angle_seg);
        }

        AngleSegment biggest_angle_seg = angle_map.getBiggestViableAngleSegment();
        if (biggest_angle_seg.getDelta() == 0)
        {
            return std::nullopt;
        }

        Angle top_angle = biggest_angle_seg.getAngleTop();
        Angle bottom_angle = biggest_angle_seg.getAngleBottom();

        Point top_point    = Point(goal_post.getStart().x(),
                                   (top_angle.sin() / top_angle.cos()) *
                                   (goal_post.getStart().x() - shot_origin.x()) +
                                   shot_origin.y());
        Point bottom_point = Point(goal_post.getStart().x(),
                                   (bottom_angle.sin() / bottom_angle.cos()) *
                                   (goal_post.getStart().x() - shot_origin.x()) +
                                   shot_origin.y());

        Point shot_point = (top_point - bottom_point) / 2 + bottom_point;

        Angle open_angle = Angle::fromDegrees(biggest_angle_seg.getDelta());
        return std::make_optional(Shot(shot_point, open_angle));
    }
    else
    {
        if (pos_post_angle.toRadians() < 0)
        {
            pos_post_angle += Angle::fromRadians(2 * M_PI);
        }
        if (neg_post_angle.toRadians() < 0)
        {
            neg_post_angle += Angle::fromRadians(2 * M_PI);
        }

        FriendlyAngleMap angle_map =
                FriendlyAngleMap(pos_post_angle, neg_post_angle, max_num_obstacles);

        for (const Robot &robot_obstacle : robot_obstacles)
        {
            Point enemy_robot_pos    = robot_obstacle.position();
            Vector perpendicular_vec = (enemy_robot_pos - shot_origin).perpendicular();

            Vector one_end_vec = perpendicular_vec.normalize(ROBOT_MAX_RADIUS_METERS);

            Point one_end   = enemy_robot_pos + one_end_vec;
            Point other_end = enemy_robot_pos - one_end_vec;

            Vector bottom_vec = one_end - shot_origin;
            Vector top_vec    = other_end - shot_origin;

            Angle top_angle = top_vec.orientation();
            if (top_angle.toRadians() < 0)
            {
                top_angle += Angle::fromRadians(2 * M_PI);
            }

            Angle bottom_angle = bottom_vec.orientation();
            if (bottom_angle.toRadians() < 0)
            {
                bottom_angle += Angle::fromRadians(2 * M_PI);
            }

            if (bottom_angle < angle_map.getAngleSegment().getAngleTop() ||
                top_angle > angle_map.getAngleSegment().getAngleBottom())
            {
                continue;
            }

            ObstacleAngleSegment non_viable_angle_seg = ObstacleAngleSegment(top_angle, bottom_angle);
            obstacles.emplace_back(non_viable_angle_seg);
        }

        std::sort(obstacles.begin(), obstacles.end(), [](ObstacleAngleSegment &a, ObstacleAngleSegment &b) -> int {
            if (a < b) {
                return true;
            } else if (a > b) {
                return false;
            }

            return false;
        });

        for (ObstacleAngleSegment &obstacle_angle_seg : obstacles) {
            angle_map.addNonViableAngleSegment(obstacle_angle_seg);
        }

        AngleSegment biggest_angle_seg = angle_map.getBiggestViableAngleSegment();
        if (biggest_angle_seg.getDelta() == 0)
        {
            return std::nullopt;
        }

        Angle top_angle = biggest_angle_seg.getAngleTop();
        if (top_angle.toRadians() > M_PI)
        {
            top_angle -= Angle::fromRadians(2 * M_PI);
        }


        Angle bottom_angle = biggest_angle_seg.getAngleBottom();
        if (bottom_angle.toRadians() > M_PI)
        {
            bottom_angle -= Angle::fromRadians(2 * M_PI);
        }

        Point top_point    = Point(goal_post.getStart().x(),
                                   (top_angle.sin() / top_angle.cos()) *
                                   (goal_post.getStart().x() - shot_origin.x()) +
                                   shot_origin.y());
        Point bottom_point = Point(goal_post.getStart().x(),
                                   (bottom_angle.sin() / bottom_angle.cos()) *
                                   (goal_post.getStart().x() - shot_origin.x()) +
                                   shot_origin.y());

        Point shot_point = (top_point - bottom_point) / 2 + bottom_point;

        Angle open_angle = Angle::fromDegrees(biggest_angle_seg.getDelta());
        return std::make_optional(Shot(shot_point, open_angle));
    }
}

std::optional<Shot> calcBestShotOnGoal(const Field &field, const Team &friendly_team,
                                       const Team &enemy_team, const Point &shot_origin,
                                       TeamType goal,
                                       const std::vector<Robot> &robots_to_ignore,
                                       double radius)
{
    std::vector<Robot> obstacles;
    std::vector<Robot> all_robots;

    size_t max_num_robots = enemy_team.numRobots() + friendly_team.numRobots() - 1 == 0 ? 1 : enemy_team.numRobots() + friendly_team.numRobots() - 1;
    all_robots.reserve(max_num_robots);
    all_robots.insert(all_robots.begin(), enemy_team.getAllRobots().begin(), enemy_team.getAllRobots().end());
    all_robots.insert(all_robots.begin(), friendly_team.getAllRobots().begin(), friendly_team.getAllRobots().end());

    for (const Robot &robot : all_robots) {
        if (std::count(robots_to_ignore.begin(), robots_to_ignore.end(), robot) == 0)
        {
            if (goal == TeamType::ENEMY)
            {
                if (robot.position().x() < shot_origin.x()) {
                    continue;
                }
            } else
            {
                if (robot.position().x() > shot_origin.x()) {
                    continue;
                }
            }

            obstacles.emplace_back(robot);
        }
    }

    if (goal == TeamType::FRIENDLY)
    {
        return calcBestShotOnGoal(
                Segment(field.friendlyGoalpostPos(), field.friendlyGoalpostNeg()),
                shot_origin, obstacles, goal, radius);
    }
    else
    {
        return calcBestShotOnGoal(
                Segment(field.enemyGoalpostPos(), field.enemyGoalpostNeg()), shot_origin,
                obstacles, goal, radius);
    }
}