#version 460 core

in float instanceID;

out float id;

void main()
{
    id = instanceID;
}