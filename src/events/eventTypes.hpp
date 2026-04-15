
#ifndef EVENT_TYPES_HPP
#define EVENT_TYPES_HPP


template<typename type>
struct CollisionAgainstWall {
    type& Wall;             // Entidad Pared
    type& AgainstWall;      // Entidad Colisionada
};

struct ClickEvent {
    int idButton;
};

template<typename type>
struct HoverEvent {
    type Button;            // Entidad Botón
};
#endif // EVENT_TYPES_HPP
